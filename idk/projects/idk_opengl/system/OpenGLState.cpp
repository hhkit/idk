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

		brdf_texture = Core::GetResourceManager().Create<OpenGLTexture>();
		brdf_texture->Bind();
		auto m =brdf_texture->GetMeta();
		m.internal_format = ColorFormat::RGF_16;
		brdf_texture->SetMeta(m);
		brdf_texture->Size(ivec2{ 512 });
	}


	struct MaterialVisitor
	{
		PipelineProgram& pipeline;
		GLuint& texture_units;
		string_view uniform_id;

		template<typename T> auto operator()(const T& elem) const
		{
			if constexpr (std::is_same_v<T, RscHandle<Texture>>)
			{
				auto texture = RscHandle<ogl::OpenGLTexture>{ elem };
				texture->BindToUnit(texture_units);
				pipeline.SetUniform(uniform_id, texture_units);

				++texture_units;
			}
			else
				pipeline.SetUniform(uniform_id, elem);
		}
	};

	void OpenGLState::RenderDrawBuffer()
	{
		auto& object_buffer = sys->object_buffer;
		auto& curr_write_buffer = sys->curr_write_buffer;
		auto& curr_draw_buffer = sys->curr_draw_buffer;
		curr_draw_buffer = curr_write_buffer;
		auto& curr_object_buffer = object_buffer[curr_draw_buffer];

		#pragma region Helper Lambdas
		const auto BindVertexShader = [this](const RscHandle<ShaderProgram>& handle, const mat4& perspective_matrix)
		{
			pipeline.PushProgram(handle);
			pipeline.SetUniform("PerCamera.perspective_transform", perspective_matrix);
		};

		const auto SetObjectUniforms = [this](const auto& object, const mat4& view_matrix)
		{
			auto obj_tfm = view_matrix * object.transform;
			pipeline.SetUniform("ObjectMat4s.object_transform", obj_tfm);
			pipeline.SetUniform("ObjectMat4s.normal_transform", obj_tfm.inverse().transpose());
		};

		const auto SetSkeletonUniforms = [this, &curr_object_buffer](const AnimatedRenderObject& render_object)
		{
			auto& skeleton = curr_object_buffer.skeleton_transforms[render_object.skeleton_index];
			for (unsigned i = 0; i < skeleton.bones_transforms.size(); ++i)
			{
				auto& transform = skeleton.bones_transforms[i];
				string bone_transform_blk = "BoneMat4s[" + std::to_string(i) + "].bone_transform";
				pipeline.SetUniform(bone_transform_blk, transform);
			}
		};

		const auto BindMaterialInstance = [this, &curr_object_buffer](const MaterialInstance& material_instance, GLuint& texture_units)
		{
			auto material = material_instance.material;
			pipeline.PushProgram(material->_shader_program);

			// material uniforms
			auto instance_uniforms = material_instance.material->uniforms;
			for (auto& [id, uniform] : material_instance.uniforms)
				instance_uniforms.emplace(id, uniform);

			for (auto& [id, uniform] : instance_uniforms)
				std::visit(MaterialVisitor{pipeline, texture_units, id}, uniform);
		};

		const auto BindPBR = [this](const std::variant<vec4, RscHandle<CubeMap>>& clear_data, const mat4& inv_view_tfm, GLuint& texture_units)
		{
			std::visit([&]([[maybe_unused]] const auto& obj)
			{
				using T = std::decay_t<decltype(obj)>;
				if constexpr (std::is_same_v<T, RscHandle<CubeMap>>)
				{
					auto opengl_handle = RscHandle<ogl::OpenGLCubemap>{ obj };
					opengl_handle->BindConvolutedToUnit(texture_units);
					pipeline.SetUniform("irradiance_probe", texture_units++);
					pipeline.SetUniform("environment_probe", opengl_handle, texture_units++);
				}
			}, clear_data);

			pipeline.SetUniform("brdfLUT", brdf_texture, texture_units++);
			pipeline.SetUniform("PerCamera.inverse_view_transform", inv_view_tfm);
		};

		const auto BindLights = [this, &curr_object_buffer](GLuint& texture_units)
		{
			auto& lights = curr_object_buffer.lights;

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
				pipeline.SetUniform(lightblk + "vp", light.vp);

				if (light.light_map)
				{
					auto t = light.light_map->GetAttachment(AttachmentType::eDepth, 0);
					pipeline.SetUniform("shadow_maps[" + std::to_string(i) + "]", RscHandle<OpenGLTexture>{t}, texture_units++);
				}
			}
		};
#pragma endregion

		pipeline.Use();
		glBindVertexArray(vao_id);

		/***********************************************************************************************************/
		/* SHADOW PASS                                                                                             */
		/***********************************************************************************************************/
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		for (const auto& elem : curr_object_buffer.lights)
		{
			if (elem.index == 0) // point light
				Core::GetSystem<DebugRenderer>().Draw(sphere{ elem.v_pos, 0.1f }, elem.light_color);

			if (elem.index == 1) // directional light
			{
				Core::GetSystem<DebugRenderer>().Draw(ray{ elem.v_pos, elem.v_dir * 0.25f }, elem.light_color);

				fb_man.SetRenderTarget(RscHandle<FrameBuffer>{elem.light_map});

				const auto light_view_tfm = elem.v;
				const auto light_p_tfm = elem.p; //near and far is currently hardcoded

				pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FShadow]);

				BindVertexShader(renderer_vertex_shaders[VertexShaders::NormalMesh], light_p_tfm);
				for (auto& elem : curr_object_buffer.mesh_render)
				{
					SetObjectUniforms(elem, light_view_tfm);
					RscHandle<OpenGLMesh>{ elem.mesh }->BindAndDraw<MeshRenderer>();
				}

				BindVertexShader(renderer_vertex_shaders[SkinnedMesh], light_p_tfm);
				for (auto& elem : curr_object_buffer.skinned_mesh_render)
				{
					// Setting bone transforms
					SetSkeletonUniforms(elem);
					SetObjectUniforms(elem, light_view_tfm);
					RscHandle<OpenGLMesh>{ elem.mesh }->BindAndDraw<SkinnedMeshRenderer>();
				}
			}
		}

		/***********************************************************************************************************/
		/* DRAWING PASS                                                                                            */
		/***********************************************************************************************************/
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

			fb_man.SetRenderTarget(RscHandle<FrameBuffer>{cam.render_target});
			
			// draw skybox if present
			std::visit([&]([[maybe_unused]] const auto& obj)
			{
				if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, RscHandle<CubeMap>>)
				{
					if (!obj)
						return;

					glDisable(GL_CULL_FACE);
					glDepthMask(GL_FALSE);

					pipeline.PushProgram(renderer_vertex_shaders[SkyBox]);
					pipeline.PushProgram(renderer_fragment_shaders[FSkyBox]);

					pipeline.SetUniform("PerCamera.pv_transform", cam.projection_matrix * mat4(mat3(cam.view_matrix)));
					pipeline.SetUniform("sb", RscHandle<OpenGLCubemap>{obj}, 0);

					RscHandle<OpenGLMesh>{Mesh::defaults[MeshType::Box]}->BindAndDraw({{ std::make_pair(vtx::Attrib::Position, 0) }});

					glDepthMask(GL_TRUE);
					glEnable(GL_CULL_FACE);
				}
				else
					glClearColor(obj.x, obj.y, obj.z, obj.w);
			}, cam.clear_data);

			// render debug
			BindVertexShader(renderer_vertex_shaders[VertexShaders::Debug], cam.projection_matrix);
			pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FDebug]);

			if (cam.render_target->GetMeta().render_debug && cam.render_target->GetMeta().is_world_renderer)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				for (auto& elem : Core::GetSystem<DebugRenderer>().GetWorldDebugInfo())
				{
					pipeline.SetUniform("ColorBlk.color", elem.color.as_vec3);
					SetObjectUniforms(elem, cam.view_matrix);
					RscHandle<OpenGLMesh>{ elem.mesh }->BindAndDraw<MeshRenderer>();
				}
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}

			// per mesh render
			BindVertexShader(renderer_vertex_shaders[VertexShaders::NormalMesh], cam.projection_matrix);

			for (auto& elem : curr_object_buffer.mesh_render)
			{
				GLuint texture_units = 0;

				BindMaterialInstance(*elem.material_instance, texture_units);
				BindPBR(cam.clear_data, inv_view_tfm, texture_units);
				BindLights(texture_units);
				SetObjectUniforms(elem, cam.view_matrix);
				RscHandle<OpenGLMesh>{ elem.mesh }->BindAndDraw<MeshRenderer>();
			}

			BindVertexShader(renderer_vertex_shaders[VertexShaders::SkinnedMesh], cam.projection_matrix);
			for (auto& elem : curr_object_buffer.skinned_mesh_render)
			{
				GLuint texture_units = 0;

				BindMaterialInstance(*elem.material_instance, texture_units);
				BindPBR(cam.clear_data, inv_view_tfm, texture_units);
				BindLights(texture_units);
				SetSkeletonUniforms(elem);
				SetObjectUniforms(elem, cam.view_matrix);
				RscHandle<OpenGLMesh>{ elem.mesh }->BindAndDraw<SkinnedMeshRenderer>();
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


		pipeline.PushProgram(*Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/pbr_convolute.vert", false));
		pipeline.PushProgram(*Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/single_pass_cube.geom", false));
		pipeline.PushProgram(Core::GetSystem<GraphicsSystem>().convoluter);

		static const auto perspective_matrix = perspective(deg{ 90 }, 1.f, 0.1f, 100.f);
		
		static const mat4 view_matrices[] =
		{
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3( 1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)))},
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3(-1.0f,  0.0f,  0.0f), vec3(0.0f, -1.0f,  0.0f)))},
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3( 0.0f,  1.0f,  0.0f), vec3(0.0f,  0.0f,  1.0f)))},
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3( 0.0f, -1.0f,  0.0f), vec3(0.0f,  0.0f, -1.0f)))},
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3( 0.0f,  0.0f,  1.0f), vec3(0.0f, -1.0f,  0.0f)))},
			mat4{invert_rotation(look_at(vec3(0.0f, 0.0f, 0.0f), vec3( 0.0f,  0.0f, -1.0f), vec3(0.0f, -1.0f,  0.0f)))}
		};

		pipeline.SetUniform("Mat4Blk.perspective_mtx", perspective_matrix);
		for (auto i = 0; i < 6; ++i)
			pipeline.SetUniform("Mat4Blk.pv_matrices[" + std::to_string(i) + "]", view_matrices[i]);

		pipeline.SetUniform("environment_probe", handle, 0);

		RscHandle<ogl::OpenGLMesh>{ Mesh::defaults[MeshType::Box] }
		->BindAndDraw({ {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal, 1),
			std::make_pair(vtx::Attrib::UV, 2)
			} });

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		pipeline.PopAllPrograms();
		fb_man.ResetFramebuffer();
	}

	void OpenGLState::ComputeBRDF(const RscHandle<ogl::Program>& handle)
	{
		pipeline.Use();
		glBindVertexArray(vao_id);
		fb_man.SetRenderTarget(brdf_texture);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);


		pipeline.PushProgram(*Core::GetResourceManager().Load<ShaderProgram>("/assets/shader/fsq.vert", false));
		pipeline.PushProgram(RscHandle<ShaderProgram>{handle});

		RscHandle<ogl::OpenGLMesh>{ Mesh::defaults[MeshType::FSQ] }
		->BindAndDraw({ {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::UV, 1),
			} });

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		pipeline.PopAllPrograms();
		fb_man.ResetFramebuffer();
	}

}
