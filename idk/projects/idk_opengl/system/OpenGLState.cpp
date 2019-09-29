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

		renderer_fragment_shaders[Debug]     = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/debug.frag");
		renderer_fragment_shaders[SkyBox]    = *Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/skybox.frag");
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
				Core::GetSystem<DebugRenderer>().Draw(ray{ elem.v_pos, elem.v_dir * 0.1f }, elem.light_color);
		}

		// range over cameras
		for(auto cam: curr_object_buffer.camera)
		{
			//fb_man.SetRenderTarget({});
			fb_man.SetRenderTarget(RscHandle<FrameBuffer>{cam.render_target});
			//Bind frame buffers based on the camera's render target
			//Set the clear color according to the camera
			
			const auto inv_view_tfm = invert_rotation(cam.view_matrix);

			std::visit([&]([[maybe_unused]] const auto& obj)
			{
				if constexpr(std::is_same_v<std::decay_t<decltype(obj)>, RscHandle<CubeMap>>)
				{
					auto& oglCubeMap = std::get<RscHandle<CubeMap>>(cam.clear_data).as<OpenGLCubemap>();

					//oglCubeMap.ID;

					glDepthMask(GL_FALSE);
					pipeline.PushProgram(renderer_vertex_shaders[SkyBox]);
					pipeline.PushProgram(renderer_fragment_shaders[SkyBox]);

					pipeline.SetUniform("PerCamera.pv_transform", cam.projection_matrix * mat4(mat3(cam.view_matrix)));


					auto& mesh = (*cam.CubeMapMesh).as<OpenGLMesh>();
					mesh.Bind(renderer_reqs
						{ {
							std::make_pair(vtx::Attrib::Position, 0)
						} });
					oglCubeMap.Bind();
					mesh.Draw();
					glDepthMask(GL_TRUE);

				}
				if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, vec4>)
				{
					glClearColor(obj.x, obj.y, obj.z, obj.w);
				}
			}, cam.clear_data);

			// calculate lights for this camera
			auto lights = curr_object_buffer.lights;
			for (auto& elem : lights)
			{
				elem.v_pos = vec3{ cam.view_matrix * vec4{ elem.v_pos, 1 } };
				elem.v_dir = vec3{ cam.view_matrix * vec4{ elem.v_dir , 0 } };
			}
			
			// lock drawing buffer
			pipeline.Use();
			glBindVertexArray(vao_id);

			// render default meshes

			pipeline.PushProgram(renderer_vertex_shaders[VertexShaders::Debug]);
			pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FDebug]);

			pipeline.SetUniform("PerCamera.perspective_transform", cam.projection_matrix);
			// render debug
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (cam.render_target->GetMeta().render_debug)
			{
				if (cam.render_target->GetMeta().is_world_renderer)
				{
					for (auto& elem : Core::GetSystem<DebugRenderer>().GetWorldDebugInfo())
					{
						auto& mesh = elem.mesh.as<OpenGLMesh>();
						mesh.Bind(MeshRenderer::GetRequiredAttributes());

						auto obj_tfm = cam.view_matrix * elem.transform;
						pipeline.SetUniform("ObjectMat4s.object_transform", obj_tfm);
						pipeline.SetUniform("ObjectMat4s.normal_transform", obj_tfm.inverse().transpose());
						pipeline.SetUniform("ColorBlk.color", elem.color.as_vec3);
						//	elem.duration == elem.duration.zero() 
						//	? 1.f 
						//	: (elem.duration - elem.display_time) / elem.duration);
						mesh.Draw();
					}
				}
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
							opengl_handle->BindToUnit(texture_units);
							pipeline.SetUniform("irradiance_probe", texture_units);
							pipeline.SetUniform("environment_probe", texture_units);
							++texture_units;
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
							opengl_handle->BindToUnit(texture_units);
							pipeline.SetUniform("irradiance_probe", texture_units);
							pipeline.SetUniform("environment_probe", texture_units);
							++texture_units;
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

}
