#include "pch.h"

#include <gfx/DebugRenderer.h>
#include <gfx/MeshRenderer.h>

#include <opengl/resource/OpenGLMesh.h>
#include <opengl/resource/OpenGLTexture.h>
#include <opengl/resource/FrameBuffer.h>
#include <gfx/FrameBuffer.h>

#include <core/Core.h>
#include <file/FileSystem.h>
#include <math/matrix_transforms.h>
#include <opengl/system/OpenGLGraphicsSystem.h>
#include "OpenGLState.h"
#include <anim/SkinnedMeshRenderer.h>
#include <iostream>
#include <gfx/CubeMap.h>
#include <opengl/resource/OpenGLCubemap.h>

#include <editor/IDE.h>
#include <gfx/ViewportUtil.h>

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
		renderer_vertex_shaders[Debug]       = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/debug.vert");
		renderer_vertex_shaders[NormalMesh]  = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/mesh.vert");
		renderer_vertex_shaders[SkinnedMesh] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skinned_mesh.vert");
		renderer_vertex_shaders[SkyBox]      = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skybox.vert");
		renderer_vertex_shaders[Particle]    = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/particle.vert");

		renderer_fragment_shaders[FDebug] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/debug.frag");
		renderer_fragment_shaders[FSkyBox] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skybox.frag");
		renderer_fragment_shaders[FShadow] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/shadow.frag");
		renderer_fragment_shaders[FPicking] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/picking.frag");

		brdf_texture = Core::GetResourceManager().Create<OpenGLTexture>();
		brdf_texture->Bind();
		auto m =brdf_texture->GetMeta();
		m.internal_format = ColorFormat::RGF_16;
		brdf_texture->SetMeta(m);
		brdf_texture->Size(ivec2{ 512 });

		fb_man.cBufferPickingTexture = Core::GetResourceManager().Create<OpenGLTexture>();
		fb_man.cBufferPickingTexture->Bind();
		auto pickMeta = fb_man.cBufferPickingTexture->GetMeta();
		pickMeta.internal_format = ColorFormat::RUI_32;
		pickMeta.format = InputChannels::RGBA;
		fb_man.cBufferPickingTexture->SetMeta(pickMeta);
		//fb_man.cBufferPickingTexture->Buffer(nullptr, main_buffer->GetMeta().size, pickMeta.format, pickMeta.internal_format);
		fb_man.cBufferPickingTexture->Size(ivec2{ 512 });
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

		for (auto& cam : curr_object_buffer.camera)
		{
			auto& rt = *cam.render_target;
			if (rt.NeedsFinalizing())
				rt.Finalize();
		}


		assert(Core::GetSystem<GraphicsSystem>().brdf);
		ComputeBRDF(RscHandle<Program>{Core::GetSystem<GraphicsSystem>().brdf});

		const auto BindVertexShader = [this](RscHandle<ShaderProgram> vertex_shader, const mat4& projection_mtx, const mat4& view_mtx)
		{
			pipeline.PushProgram(vertex_shader);
			pipeline.SetUniform("PerCamera.perspective_transform", projection_mtx);
			pipeline.SetUniform("PerCamera.view_transform", view_mtx);
		};

		const auto SetObjectUniforms = [this](const auto& render_obj, const mat4& view_matrix)
		{
			const auto obj_tfm = view_matrix * render_obj.transform;
			pipeline.SetUniform("ObjectMat4s.object_transform", obj_tfm);
			pipeline.SetUniform("ObjectMat4s.normal_transform", obj_tfm.inverse().transpose());
			pipeline.SetUniform("ObjectMat4s.model_transform", render_obj.transform);
		};

		const auto SetSkeletons = [this, &curr_object_buffer](int skeleton_index)
		{
			auto& skeleton = curr_object_buffer.skeleton_transforms[skeleton_index];
			for (unsigned i = 0; i < skeleton.bones_transforms.size(); ++i)
			{
				auto& transform = skeleton.bones_transforms[i];
				string bone_transform_blk = "BoneMat4s.bone_transform[" + std::to_string(i) + "]";
				pipeline.SetUniform(bone_transform_blk, transform);
			}
		};

		const auto SetLightUniforms = [this](span<LightData> lights, GLuint& texture_units)
		{
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
				pipeline.SetUniform(lightblk + "shadow_bias", light.shadow_bias);
				pipeline.SetUniform(lightblk + "cast_shadow", light.cast_shadow);
				pipeline.SetUniform(lightblk + "intensity", light.intensity);

				if (light.light_map)
				{
					const auto t = light.light_map->DepthAttachment().buffer;
					t.as<OpenGLTexture>().BindToUnit(texture_units);

					pipeline.SetUniform(lightblk + "vp", light.vp);
					(pipeline.SetUniform("shadow_maps[" + std::to_string(i) + "]", texture_units));
					texture_units++;
				}
			}
		};

		const auto SetMaterialUniforms = [this](RscHandle<MaterialInstance> material_instance, GLuint& texture_units)
		{
			for (auto& [name, uniform] : material_instance->material->uniforms)
			{
                auto val = *material_instance->GetUniform(name);
				std::visit([this, &texture_units, id = uniform.name](auto& elem)
                {
					using T = std::decay_t<decltype(elem)>;
					if constexpr (std::is_same_v<T, RscHandle<Texture>>)
					{
						const auto texture = RscHandle<ogl::OpenGLTexture>{ elem };
						texture->BindToUnit(texture_units);
						pipeline.SetUniform(id, texture_units);

						++texture_units;
					}
					else
						pipeline.SetUniform(id, elem);
				}, val);
			}
			for (auto& uniform : material_instance->material->hidden_uniforms)
			{
				std::visit([this, &texture_units, id = uniform.name](auto& elem)
                {
					using T = std::decay_t<decltype(elem)>;
					if constexpr (std::is_same_v<T, RscHandle<Texture>>)
					{
						const auto texture = RscHandle<ogl::OpenGLTexture>{ elem };
						texture->BindToUnit(texture_units);
						pipeline.SetUniform(id, texture_units);

						++texture_units;
					}
					else
						pipeline.SetUniform(id, elem);
				}, uniform.value);
			}
		};

		const auto SetPBRUniforms = [this](CameraData& cam, const mat4& inv_view_tfm, GLuint& texture_units)
		{
			std::visit([&]([[maybe_unused]] const auto& obj)
				{
					using T = std::decay_t<decltype(obj)>;
					if constexpr (std::is_same_v<T, RscHandle<CubeMap>>)
					{
						const auto opengl_handle = RscHandle<ogl::OpenGLCubemap>{ obj };
						opengl_handle->BindConvolutedToUnit(texture_units);
						pipeline.SetUniform("irradiance_probe", texture_units++);
						opengl_handle->BindToUnit(texture_units);
						pipeline.SetUniform("environment_probe", texture_units++);
					}
				}, cam.clear_data);

			brdf_texture->BindToUnit(texture_units);
			pipeline.SetUniform("brdfLUT", texture_units++);
			pipeline.SetUniform("PerCamera.inverse_view_transform", inv_view_tfm);
		};

		for (const auto& elem : curr_object_buffer.lights)
		{
			pipeline.Use();
			if (elem.index == 0) // point light
				Core::GetSystem<DebugRenderer>().Draw(sphere{ elem.v_pos, 0.1f }, elem.light_color);

			if (elem.index == 1) // directional light
			{
				Core::GetSystem<DebugRenderer>().Draw(ray{ elem.v_pos, elem.v_dir * 0.25f }, elem.light_color);
				fb_man.SetRenderTarget(s_cast<RscHandle<OpenGLFrameBuffer>>(elem.light_map));

				glClearColor(1.f,1.f,1.f,1.f);
				glClearDepth(1.f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);

				glCullFace(GL_FRONT); //Fix for peterpanning

				const auto& light_view_tfm = elem.v;
				const auto& light_p_tfm = elem.p; //near and far is currently hardcoded
				pipeline.PopAllPrograms();

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				BindVertexShader(renderer_vertex_shaders[VertexShaders::NormalMesh], light_p_tfm, light_view_tfm);

				for (auto& render_obj : curr_object_buffer.mesh_render)
				{
					pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FShadow]);
					SetObjectUniforms(render_obj, light_view_tfm);
					RscHandle<OpenGLMesh>{render_obj.mesh}->BindAndDraw<MeshRenderer>();
				}

				BindVertexShader(renderer_vertex_shaders[VertexShaders::SkinnedMesh], light_p_tfm, light_view_tfm);
				for (auto& render_obj : curr_object_buffer.skinned_mesh_render)
				{
					pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FShadow]);
					SetSkeletons(render_obj.skeleton_index);
					SetObjectUniforms(render_obj, light_view_tfm);
					RscHandle<OpenGLMesh>{render_obj.mesh}->BindAndDraw<SkinnedMeshRenderer>();
				}

				glDisable(GL_DEPTH_TEST);
				glCullFace(GL_BACK);

				fb_man.ResetFramebuffer();
			}
		}

		glEnable(GL_DEPTH_TEST);

		RscHandle<FrameBuffer> main_buffer;
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
				//if (cam.obj_id == curr_object_buffer.curr_scene_camera.obj_id)
				//{
				//	main_buffer = RscHandle<OpenGLRenderTarget>{ cam.render_target };
				//	fb_man.SetRenderTarget(main_buffer);
				//}
				//else
				//auto position = ivec2{vec2{ cam.viewport.position } *vec2{ cam.render_target->Size() }};
				//auto size =     ivec2{vec2{ cam.viewport.size     } *vec2{ cam.render_target->Size() }};
				fb_man.SetRenderTarget(RscHandle<OpenGLRenderTarget>{cam.render_target},cam.viewport,cam.clear_data.index() != idk::index_in_variant_v<DontClear,decltype(cam.clear_data)>);
			}
			
			// lock drawing buffer
			pipeline.Use();
			pipeline.PopAllPrograms();
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

					oglCubeMap.BindToUnit(0);
					pipeline.SetUniform("sb", 0);
					RscHandle<OpenGLMesh>{*cam.CubeMapMesh}->BindAndDraw(renderer_reqs
						{ {
							std::make_pair(vtx::Attrib::Position, 0)
						} });

					glDepthMask(GL_TRUE);
					glEnable(GL_CULL_FACE);
				}
				if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, color>)
					glClearColor(obj.x, obj.y, obj.z, obj.w);
			}, cam.clear_data);

			BindVertexShader(renderer_vertex_shaders[VertexShaders::Debug], cam.projection_matrix, cam.view_matrix);
			pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FDebug]);
			// render debug
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (cam.overlay_debug_draw && cam.render_target->RenderDebug() && cam.render_target->IsWorldRenderer())
				for (auto& elem : Core::GetSystem<DebugRenderer>().GetWorldDebugInfo())
				{
					SetObjectUniforms(elem, cam.view_matrix);
					pipeline.SetUniform("ColorBlk.color", elem.color.as_vec3);

					RscHandle<OpenGLMesh>{elem.mesh}->BindAndDraw<MeshRenderer>();
				}

			// per mesh render
			BindVertexShader(renderer_vertex_shaders[VertexShaders::NormalMesh], cam.projection_matrix, cam.view_matrix);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			for (auto& elem : curr_object_buffer.mesh_render)
			{
				// bind shader
				const auto material = elem.material_instance->material;
				pipeline.PushProgram(material->_shader_program);

				// set probe
				GLuint texture_units = 0;
				SetPBRUniforms     (cam, inv_view_tfm, texture_units);
				SetLightUniforms   (span<LightData>{lights}, texture_units);
				SetMaterialUniforms(elem.material_instance, texture_units);
				SetObjectUniforms  (elem, cam.view_matrix);

				RscHandle<OpenGLMesh>{elem.mesh}->BindAndDraw<MeshRenderer>();
			}

			BindVertexShader(renderer_vertex_shaders[VertexShaders::SkinnedMesh], cam.projection_matrix, cam.view_matrix);
			for (auto& elem : curr_object_buffer.skinned_mesh_render)
			{
				// bind shader
				const auto material = elem.material_instance->material;
				pipeline.PushProgram(material->_shader_program);

				GLuint texture_units = 0;
				SetPBRUniforms     (cam, inv_view_tfm, texture_units);
				SetLightUniforms   (span<LightData>{lights}, texture_units);
				SetSkeletons       (elem.skeleton_index);
				SetMaterialUniforms(elem.material_instance, texture_units);
				SetObjectUniforms  (elem, cam.view_matrix);

				RscHandle<OpenGLMesh>{elem.mesh}->BindAndDraw<SkinnedMeshRenderer>();
			}


            static vector<OpenGLBuffer> bufs = []()
            {
                vector<OpenGLBuffer> bufs;
                unsigned int indices[]{ 0, 3, 1, 1, 3, 2 };
                bufs.emplace_back(OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} })
                    .Bind().Buffer(indices, sizeof(int), 6);
                bufs.emplace_back(OpenGLBuffer{ GL_ARRAY_BUFFER, { { vtx::Attrib::ParticlePosition, 0, 0 } } });
                bufs.emplace_back(OpenGLBuffer{ GL_ARRAY_BUFFER, { { vtx::Attrib::ParticleRotation, 0, 0 } } });
                bufs.emplace_back(OpenGLBuffer{ GL_ARRAY_BUFFER, { { vtx::Attrib::ParticleSize, 0, 0 } } });
                bufs.emplace_back(OpenGLBuffer{ GL_ARRAY_BUFFER, { { vtx::Attrib::Color, 0, 0 } } });
                return bufs;
            }();

            BindVertexShader(renderer_vertex_shaders[VertexShaders::Particle], cam.projection_matrix, cam.view_matrix);
            auto x = glGetError();
            for (auto& elem : curr_object_buffer.particle_render_data)
            {
                // bind shader
                const auto material = elem.material_instance->material;
                pipeline.PushProgram(material->_shader_program);
                auto x = glGetError();

                RscHandle<OpenGLMesh>{Mesh::defaults[MeshType::FSQ]}->Bind(
                    renderer_reqs{ {
                        { vtx::Attrib::Position, 0 },
                    }
                });
                glVertexAttribDivisor(0, 0);

                bufs[1].Bind().Buffer(elem.positions.data(), 3 * sizeof(GLfloat), elem.positions.size());
                bufs[1].BindForDraw(renderer_reqs{ { {vtx::Attrib::ParticlePosition, 1} } });
                x = glGetError();
                glVertexAttribDivisor(1, 1);

                bufs[2].Bind().Buffer(elem.rotations.data(), sizeof(GLfloat), elem.rotations.size());
                bufs[2].BindForDraw(renderer_reqs{ { {vtx::Attrib::ParticleRotation, 2} } });
                x = glGetError();
                glVertexAttribDivisor(2, 1);

                bufs[3].Bind().Buffer(elem.sizes.data(), sizeof(GLfloat), elem.sizes.size());
                bufs[3].BindForDraw(renderer_reqs{ { {vtx::Attrib::ParticleSize, 3} } });
                x = glGetError();
                glVertexAttribDivisor(3, 1);

                bufs[4].Bind().Buffer(elem.colors.data(), 4 * sizeof(GLfloat), elem.colors.size());
                bufs[4].BindForDraw(renderer_reqs{ { {vtx::Attrib::Color, 4} } });
                x = glGetError();
                glVertexAttribDivisor(4, 1);

                bufs[0].Bind();
                glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, elem.positions.size());
                x = glGetError();
            }
		}

		fb_man.ResetFramebuffer();



		//////////////////////////////////For PICKING/////////////////////////////////
		// set main scene camera
		
		//vector<OpenGLTexture> addMainBuffer;
		//addMainBuffer.emplace_back(cBufferPickingTexture);
		{		
			auto cam = curr_object_buffer.curr_scene_camera;
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
					fb_man.SetRenderTarget(fb_man.cBufferPickingTexture);
				}

				// lock drawing buffer
				pipeline.Use();
				pipeline.PopAllPrograms();
				glBindVertexArray(vao_id);

				//std::visit([&]([[maybe_unused]] const auto& obj)
				//{
				//	if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, RscHandle<CubeMap>>)
				//	{
				//		if (!obj)
				//			return;
				//		auto& oglCubeMap = std::get<RscHandle<CubeMap>>(cam.clear_data).as<OpenGLCubemap>();

				//		//oglCubeMap.ID;

				//		glDisable(GL_CULL_FACE);
				//		glDepthMask(GL_FALSE);
				//		pipeline.PushProgram(renderer_vertex_shaders[SkyBox]);
				//		pipeline.PushProgram(renderer_fragment_shaders[FPicking]);

				//		pipeline.SetUniform("PerCamera.pv_transform", cam.projection_matrix * mat4(mat3(cam.view_matrix)));
				//		pipeline.SetUniform("obj_index", cam.obj_id);

				//		oglCubeMap.BindToUnit(0);
				//		pipeline.SetUniform("sb", 0);
				//		RscHandle<OpenGLMesh>{*cam.CubeMapMesh}->BindAndDraw(renderer_reqs
				//			{ {
				//				std::make_pair(vtx::Attrib::Position, 0)
				//			} });

				//		glDepthMask(GL_TRUE);
				//		glEnable(GL_CULL_FACE);
				//	}
				//	if constexpr (std::is_same_v<std::decay_t<decltype(obj)>, color>)
				//		glClearColor(obj.x, obj.y, obj.z, obj.w);
				//}, cam.clear_data);

				BindVertexShader(renderer_vertex_shaders[VertexShaders::Debug], cam.projection_matrix, cam.view_matrix);
				pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FPicking]);
				// render debug
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				if (cam.render_target->RenderDebug() && cam.render_target->IsWorldRenderer())
					for (auto& elem : Core::GetSystem<DebugRenderer>().GetWorldDebugInfo())
					{
						SetObjectUniforms(elem, cam.view_matrix);
						pipeline.SetUniform("ColorBlk.color", elem.color.as_vec3);
						pipeline.SetUniform("obj_index", cam.obj_id);

						RscHandle<OpenGLMesh>{elem.mesh}->BindAndDraw<MeshRenderer>();
					}

				// per mesh render
				BindVertexShader(renderer_vertex_shaders[VertexShaders::NormalMesh], cam.projection_matrix, cam.view_matrix);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				for (auto& elem : curr_object_buffer.mesh_render)
				{
					// bind shader
					const auto material = elem.material_instance->material;
					pipeline.PushProgram(material->_shader_program);

					// set probe
					GLuint texture_units = 0;
					SetPBRUniforms(cam, inv_view_tfm, texture_units);
					SetLightUniforms(span<LightData>{lights}, texture_units);
					SetMaterialUniforms(elem.material_instance, texture_units);
					SetObjectUniforms(elem, cam.view_matrix);

					pipeline.SetUniform("obj_index", elem.obj_id);

					RscHandle<OpenGLMesh>{elem.mesh}->BindAndDraw<MeshRenderer>();
				}

				BindVertexShader(renderer_vertex_shaders[VertexShaders::SkinnedMesh], cam.projection_matrix, cam.view_matrix);
				for (auto& elem : curr_object_buffer.skinned_mesh_render)
				{
					// bind shader
					const auto material = elem.material_instance->material;
					pipeline.PushProgram(material->_shader_program);

					GLuint texture_units = 0;
					SetPBRUniforms(cam, inv_view_tfm, texture_units);
					SetLightUniforms(span<LightData>{lights}, texture_units);
					SetSkeletons(elem.skeleton_index);
					SetMaterialUniforms(elem.material_instance, texture_units);
					SetObjectUniforms(elem, cam.view_matrix);

					pipeline.SetUniform("obj_index", elem.obj_id);

					RscHandle<OpenGLMesh>{elem.mesh}->BindAndDraw<SkinnedMeshRenderer>();
				}
			}

			fb_man.ResetFramebuffer();
		}
		
	}

	void OpenGLState::ConvoluteCubeMap(const RscHandle<ogl::OpenGLCubemap>& handle)
	{
		pipeline.Use();
		glBindVertexArray(vao_id);
		fb_man.SetRenderTarget(handle, true);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		pipeline.PushProgram(*Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/pbr_convolute.vert", false));
		pipeline.PushProgram(*Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/single_pass_cube.geom", false));
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

		handle->BindToUnit(0);
		pipeline.SetUniform("environment_probe", 0);

		RscHandle<ogl::OpenGLMesh>{ Mesh::defaults[MeshType::Box] }->BindAndDraw(
			{ {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::Normal, 1),
			std::make_pair(vtx::Attrib::UV, 2)
			} });

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		pipeline.PopAllPrograms();
		fb_man.ResetFramebuffer();
		//glBindVertexArray(0);
	}

	void OpenGLState::ComputeBRDF(const RscHandle<ogl::Program>& handle)
	{
		pipeline.Use();
		glBindVertexArray(vao_id);
		fb_man.SetRenderTarget(brdf_texture);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		pipeline.PushProgram(*Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/fsq.vert", false));
		pipeline.PushProgram(RscHandle<ShaderProgram>{handle});

		RscHandle<ogl::OpenGLMesh>{ Mesh::defaults[MeshType::FSQ] }->BindAndDraw(
			{ {
			std::make_pair(vtx::Attrib::Position, 0),
			std::make_pair(vtx::Attrib::UV, 1),
			} });

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		pipeline.PopAllPrograms();
		fb_man.ResetFramebuffer();
		//glBindVertexArray(0);
	}

}
