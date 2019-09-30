#include "pch.h"

#include <gfx/DebugRenderer.h>
#include <gfx/MeshRenderer.h>

#include <idk_opengl/resource/OpenGLMesh.h>
#include <idk_opengl/resource/OpenGLTexture.h>
#include <idk_opengl/resource/FrameBuffer.h>

#include <core/Core.h>
#include <file/FileSystem.h>
#include <math/matrix_transforms.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include "OpenGLState.h"
#include <anim/SkinnedMeshRenderer.h>
#include <iostream>
#include <gfx/CubeMap.h>
#include <idk_opengl/resource/OpenGLCubemap.h>

void _check_gl_error(const char* file, int line) {
	GLenum err(glGetError());

	while (err != GL_NO_ERROR) {
		std::string error;

		switch (err) {
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line << '\n';
		err = glGetError();
	}
}

namespace idk::ogl
{
	void OpenGLState::Setup()
	{
		sys = &Core::GetSystem<Win32GraphicsSystem>();
		glGenVertexArrays(1, &vao_id);
	}

	void OpenGLState::GenResources()
	{
		renderer_vertex_shaders[Debug]       = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/debug.vert");
		renderer_vertex_shaders[NormalMesh]  = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/mesh.vert");
		renderer_vertex_shaders[SkinnedMesh] = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/skinned_mesh.vert");
		renderer_vertex_shaders[SkyBox]      = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/skybox.vert");

		renderer_fragment_shaders[FDebug] = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/debug.frag");
		renderer_fragment_shaders[FSkyBox] = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/skybox.frag");
		renderer_fragment_shaders[FShadow] = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/shadow.frag");
	}


	void OpenGLState::RenderDrawBuffer()
	{
		auto& object_buffer = sys->object_buffer;
		auto& curr_write_buffer = sys->curr_write_buffer;
		auto& curr_draw_buffer = sys->curr_draw_buffer;
		curr_draw_buffer = curr_write_buffer;
		auto& curr_object_buffer = object_buffer[curr_draw_buffer];

		for (const auto& elem : curr_object_buffer.lights)
		{
			if (elem.index == 0) // point light
				Core::GetSystem<DebugRenderer>().Draw(sphere{ elem.v_pos, 0.1f }, elem.light_color);

			if (elem.index == 1) // directional light
			{
				Core::GetSystem<DebugRenderer>().Draw(ray{ elem.v_pos, elem.v_dir * 0.1f }, elem.light_color);
				//fb_man.SetRenderTarget({});
				fb_man.SetRenderTarget(RscHandle<FrameBuffer>{elem.light_map});
				//Bind frame buffers based on the camera's render target
				//Set the clear color according to the camera

				const auto light_view_tfm = elem.v;
				const auto light_p_tfm = elem.p; //near and far is currently hardcoded

				// per mesh render
				pipeline.PushProgram(renderer_vertex_shaders[VertexShaders::NormalMesh]);

				// bind shader
				pipeline.SetUniform("PerCamera.perspective_transform", light_p_tfm);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				for (auto& elem : curr_object_buffer.mesh_render)
				{
					// shader uniforms
					pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FShadow]);
					// set probe
					GLuint texture_units = 0;

					// bind attribs
					auto& mesh = elem.mesh.as<OpenGLMesh>();
					mesh.Bind(MeshRenderer::GetRequiredAttributes());

					// set uniforms
					// object uniforms

					auto obj_tfm = light_view_tfm * elem.transform;
					pipeline.SetUniform("ObjectMat4s.object_transform", obj_tfm);
					pipeline.SetUniform("ObjectMat4s.normal_transform", obj_tfm.inverse().transpose());

					// material uniforms
					auto instance_uniforms = elem.material_instance->material->uniforms;
					for (auto& [id, uniform] : elem.material_instance->uniforms)
						instance_uniforms.emplace(id, uniform);

					for (auto& [id, uniform] : instance_uniforms)
					{
						std::visit([this, &id, &texture_units](auto& elem) {
							using T = std::decay_t<decltype(elem)>;
							if constexpr (std::is_same_v<T, RscHandle<Texture>>)
							{
								auto texture = RscHandle<ogl::OpenGLTexture>{ elem };
								texture->BindToUnit(texture_units);
								pipeline.SetUniform(id, texture_units);

								++texture_units;
							}
							else
								pipeline.SetUniform(id, elem);
						}, uniform);
					}

					// draw
					mesh.Draw();
				}

				pipeline.PushProgram(renderer_vertex_shaders[SkinnedMesh]);
				pipeline.SetUniform("PerCamera.perspective_transform", light_p_tfm);

				for (auto& elem : curr_object_buffer.skinned_mesh_render)
				{
					// bind shader
					pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FShadow]);

					// bind attribs
					auto& mesh = elem.mesh.as<OpenGLMesh>();
					mesh.Bind(SkinnedMeshRenderer::GetRequiredAttributes());
					//mesh.Bind(MeshRenderer::GetRequiredAttributes());

					// Setting bone transforms
					auto& skeleton = curr_object_buffer.skeleton_transforms[elem.skeleton_index];
					for (unsigned i = 0; i < skeleton.bones_transforms.size(); ++i)
					{
						auto& transform = skeleton.bones_transforms[i];
						string bone_transform_blk = "BoneMat4s[" + std::to_string(i) + "].bone_transform";
						pipeline.SetUniform(bone_transform_blk, transform);
					}

					// set uniforms
					// object uniforms

					auto obj_tfm = light_view_tfm * elem.transform;
					pipeline.SetUniform("ObjectMat4s.object_transform", obj_tfm);
					pipeline.SetUniform("ObjectMat4s.normal_transform", obj_tfm.inverse().transpose());

					// draw
					mesh.Draw();
				}
			}
		}

		// range over cameras
		for(auto cam: curr_object_buffer.camera)
		{
			const auto inv_view_tfm = invert_rotation(cam.view_matrix);
			// calculate lights for this camera
			auto lights = curr_object_buffer.lights;
			for (auto& elem : lights)
			{
				elem.v_pos = vec3{ cam.view_matrix * vec4{ elem.v_pos, 1 } };
				elem.v_dir = vec3{ cam.view_matrix * vec4{ elem.v_dir , 0 } };
			}

			{
				fb_man.SetRenderTarget(RscHandle<FrameBuffer>{cam.render_target});
			}
			
			// lock drawing buffer
			pipeline.Use();
			glBindVertexArray(vao_id);

			std::visit([&]([[maybe_unused]] const auto& obj)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, RscHandle<CubeMap>>)
				{
					if (!obj)
						return;
					auto& oglCubeMap = std::get<RscHandle<CubeMap>>(cam.clear_data).as<OpenGLCubemap>();

					//oglCubeMap.ID;

					glDisable(GL_CULL_FACE);
					glDepthMask(GL_FALSE);
					pipeline.PushProgram(renderer_vertex_shaders[SkyBox]);
					pipeline.PushProgram(renderer_fragment_shaders[FSkyBox]);

					pipeline.SetUniform("PerCamera.pv_transform", cam.projection_matrix * mat4(mat3(cam.view_matrix)));


					auto& mesh = (*cam.CubeMapMesh).as<OpenGLMesh>();
					mesh.Bind(renderer_reqs
						{ {
							std::make_pair(vtx::Attrib::Position, 0)
						} });
					oglCubeMap.BindToUnit(0);
					pipeline.SetUniform("sb", 0);
					mesh.Draw();
					glDepthMask(GL_TRUE);
					glEnable(GL_CULL_FACE);
				}
				if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, color>)
					glClearColor(obj.x, obj.y, obj.z, obj.w);
			}, cam.clear_data);

			pipeline.PushProgram(renderer_vertex_shaders[VertexShaders::Debug]);
			pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FDebug]);

			pipeline.SetUniform("PerCamera.perspective_transform", cam.projection_matrix);
			// render debug
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (cam.render_target->GetMeta().render_debug && cam.render_target->GetMeta().is_world_renderer)
				for (auto& elem : Core::GetSystem<DebugRenderer>().GetWorldDebugInfo())
				{
					auto& mesh = elem.mesh.as<OpenGLMesh>();
					mesh.Bind(MeshRenderer::GetRequiredAttributes());

					auto obj_tfm = cam.view_matrix * elem.transform;
					pipeline.SetUniform("ObjectMat4s.object_transform", obj_tfm);
					pipeline.SetUniform("ObjectMat4s.normal_transform", obj_tfm.inverse().transpose());
					pipeline.SetUniform("ColorBlk.color", elem.color.as_vec3);

					mesh.Draw();
				}

			// per mesh render
			pipeline.PushProgram(renderer_vertex_shaders[VertexShaders::NormalMesh]);
			pipeline.SetUniform("PerCamera.perspective_transform", cam.projection_matrix);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			for (auto& elem : curr_object_buffer.mesh_render)
			{
				// bind shader
				auto material = elem.material_instance->material;
				pipeline.PushProgram(material->_shader_program);

				// shader uniforms

				// set probe
				GLuint texture_units = 0;

				std::visit([&]([[maybe_unused]] const auto& obj)
					{
						using T = std::decay_t<decltype(obj)>;
						if constexpr (std::is_same_v<T, RscHandle<CubeMap>>)
						{
							auto opengl_handle = RscHandle<ogl::OpenGLCubemap>{ obj };
							opengl_handle->BindConvolutedToUnit(texture_units);
							pipeline.SetUniform("irradiance_probe", texture_units++);
							opengl_handle->BindToUnit(texture_units);
							pipeline.SetUniform("environment_probe", texture_units++);
						}
					}, cam.clear_data);
				auto brdf = Core::GetResourceManager().Load("/assets/textures/brdf/ibl_brdf_lut.png", false)->Get<ogl::OpenGLTexture>();
				brdf->BindToUnit(texture_units);
				pipeline.SetUniform("brdfLUT", texture_units);
				texture_units++;
				pipeline.SetUniform("PerCamera.inverse_view_transform", inv_view_tfm);

				pipeline.SetUniform("LightBlk.light_count", (int)lights.size());
				for (unsigned i = 0; i < lights.size(); ++i)
				{
					auto& light = lights[i];
					string lightblk = "LightBlk.lights[" + std::to_string(i) + "].";
					pipeline.SetUniform(lightblk + "type",      light.index);
					pipeline.SetUniform(lightblk + "color",     light.light_color.as_vec3);
					pipeline.SetUniform(lightblk + "v_pos",     light.v_pos);
					pipeline.SetUniform(lightblk + "v_dir",     light.v_dir);
					pipeline.SetUniform(lightblk + "cos_inner", light.cos_inner);
					pipeline.SetUniform(lightblk + "cos_outer", light.cos_outer);
					pipeline.SetUniform(lightblk + "vp"       , light.vp);

					auto t = light.light_map->GetAttachment(AttachmentType::eDepth, 0);
					t.as<OpenGLTexture>().BindToUnit(texture_units);
					pipeline.SetUniform("shadow_maps[" + std::to_string(i) + "]", texture_units);

					texture_units += static_cast<bool>(light.light_map);
				}

				// bind attribs
				auto& mesh = elem.mesh.as<OpenGLMesh>();
				mesh.Bind(MeshRenderer::GetRequiredAttributes());

				// set uniforms
				// object uniforms
				auto obj_tfm = cam.view_matrix * elem.transform;
				pipeline.SetUniform("ObjectMat4s.object_transform", obj_tfm);
				pipeline.SetUniform("ObjectMat4s.normal_transform", obj_tfm.inverse().transpose());

				// material uniforms
                auto instance_uniforms = elem.material_instance->material->uniforms;
                for (auto& [id, uniform] : elem.material_instance->uniforms)
                    instance_uniforms.emplace(id, uniform);

                for (auto& [id, uniform] : instance_uniforms)
				{
					std::visit([this, &id, &texture_units](auto& elem) {
						using T = std::decay_t<decltype(elem)>;
						if constexpr (std::is_same_v<T, RscHandle<Texture>>)
						{
							auto texture = RscHandle<ogl::OpenGLTexture>{ elem };
							texture->BindToUnit(texture_units);
							pipeline.SetUniform(id, texture_units);

							++texture_units;
						}
						else
							pipeline.SetUniform(id, elem);
					}, uniform);
				}

				// draw
				mesh.Draw();
			}

			pipeline.PushProgram(renderer_vertex_shaders[SkinnedMesh]);
			pipeline.SetUniform("PerCamera.perspective_transform", cam.projection_matrix);
			for (auto& elem : curr_object_buffer.skinned_mesh_render)
			{
				// bind shader
				auto material = elem.material_instance->material;
				pipeline.PushProgram(material->_shader_program);

				GLuint texture_units = 0;
				// bind probe
				std::visit([&]([[maybe_unused]] const auto& obj)
					{
						using T = std::decay_t<decltype(obj)>;
						if constexpr (std::is_same_v<T, RscHandle<CubeMap>>)
						{
							auto opengl_handle = RscHandle<ogl::OpenGLCubemap>{ obj };
							opengl_handle->BindConvolutedToUnit(texture_units);
							pipeline.SetUniform("irradiance_probe", texture_units++);
							opengl_handle->BindToUnit(texture_units);
							pipeline.SetUniform("environment_probe", texture_units++);
						}
					}, cam.clear_data);

				// shader uniforms
				pipeline.SetUniform("LightBlk.light_count", (int)lights.size());

				for (unsigned i = 0; i < lights.size(); ++i)
				{
					auto& light = lights[i];
					string lightblk = "LightBlk.lights[" + std::to_string(i) + "].";
					pipeline.SetUniform(lightblk + "type", light.index);
					pipeline.SetUniform(lightblk + "color", light.light_color.as_vec3);
					pipeline.SetUniform(lightblk + "v_pos", light.v_pos);
					pipeline.SetUniform(lightblk + "v_dir", light.v_dir);
					pipeline.SetUniform(lightblk + "cos_inner", light.cos_inner);
					pipeline.SetUniform(lightblk + "cos_outer", light.cos_outer);

					texture_units += static_cast<bool>(light.light_map);
				}
				// bind attribs
				auto& mesh = elem.mesh.as<OpenGLMesh>();
				mesh.Bind(SkinnedMeshRenderer::GetRequiredAttributes());
				//mesh.Bind(MeshRenderer::GetRequiredAttributes());

				// Setting bone transforms
				auto& skeleton = curr_object_buffer.skeleton_transforms[elem.skeleton_index];
				for (unsigned i = 0; i < skeleton.bones_transforms.size(); ++i)
				{
					auto& transform = skeleton.bones_transforms[i];
					string bone_transform_blk = "BoneMat4s[" + std::to_string(i) + "].bone_transform";
					pipeline.SetUniform(bone_transform_blk, transform);
				}

				// set uniforms
				// object uniforms

				auto obj_tfm = cam.view_matrix * elem.transform;
				pipeline.SetUniform("ObjectMat4s.object_transform", obj_tfm);
				pipeline.SetUniform("ObjectMat4s.normal_transform", obj_tfm.inverse().transpose());

				// material uniforms
                auto instance_uniforms = elem.material_instance->material->uniforms;
                for (auto& [id, uniform] : elem.material_instance->uniforms)
                    instance_uniforms.emplace(id, uniform);

				for (auto& [id, uniform] : instance_uniforms)
				{
					std::visit([this, &id, &texture_units](auto& elem) {
						using T = std::decay_t<decltype(elem)>;
						if constexpr (std::is_same_v<T, RscHandle<Texture>>)
						{
							auto texture = RscHandle<ogl::OpenGLTexture>{ elem };
							texture->BindToUnit(texture_units);
							pipeline.SetUniform(id, texture_units);

							++texture_units;
						}
						else
							pipeline.SetUniform(id, elem);
						}, uniform);
				}

				// draw
				mesh.Draw();
			}
		}

		fb_man.ResetFramebuffer();
	}

	void OpenGLState::ConvoluteCubeMap(const RscHandle<ogl::OpenGLCubemap>& handle)
	{
		pipeline.Use();
		glBindVertexArray(vao_id);
		fb_man.SetRenderTarget(handle, true);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		auto box_mesh = RscHandle<ogl::OpenGLMesh>{ Mesh::defaults[MeshType::Box] };

		pipeline.PushProgram(*Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/pbr_convolute.vert", false));
		pipeline.PushProgram(*Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/single_pass_cube.geom", false));
		pipeline.PushProgram(Core::GetSystem<GraphicsSystem>().convoluter);

		static const auto perspective_matrix = perspective(deg{ 90 }, 1.f, 0.1f, 100.f);
		
		static const mat4 view_matrices[] =
		{
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)))},
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f,  0.0f,  0.0f),vec3(0.0f, -1.0f,  0.0f)))},
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  1.0f,  0.0f), vec3(0.0f,  0.0f,  1.0f)))},
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, -1.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f)))},
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f,  1.0f), vec3(0.0f, -1.0f,  0.0f)))},
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f,  0.0f, -1.0f), vec3(0.0f, -1.0f,  0.0f)))}
		};

		pipeline.SetUniform("Mat4Blk.perspective_mtx", perspective_matrix);
		for (auto i = 0; i < 6; ++i)
			pipeline.SetUniform("Mat4Blk.pv_matrices[" + std::to_string(i) + "]", view_matrices[i]);

		handle->BindToUnit(0);
		pipeline.SetUniform("environment_probe", 0);

		box_mesh->Bind(
			{ {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal, 1),
			std::make_pair(vtx::Attrib::UV, 2)
			} });
		box_mesh->Draw();
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		pipeline.PopAllPrograms();
		fb_man.ResetFramebuffer();
	}

}
