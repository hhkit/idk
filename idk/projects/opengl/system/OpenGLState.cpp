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
#include <gfx/CubeMap.h>
#include <opengl/resource/OpenGLCubemap.h>
#include <math/shapes/frustum.h>
#include <gfx/FramebufferFactory.h>
#include <gfx/FontAtlas.h>
#include <opengl/resource/OpenGLFontAtlas.h>

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

		LOG_ERROR_TO(idk::LogPool::GFX, "GL_%s-%s: %d", error.c_str(), file, line);
		err = glGetError();
	}
}

namespace idk::ogl
{
	void OpenGLState::Setup()
	{
		sys = &Core::GetSystem<Win32GraphicsSystem>();
		glGenVertexArrays(1, &particle_vao_id);
		glGenVertexArrays(1, &font_vao_id);
		glGenVertexArrays(1, &vao_id);	

		glGenBuffers(1, &vbo_font_id);
		glGenBuffers(1, &object_vbo_id);
		glGenBuffers(1, &normal_vbo_id);
	}

	OpenGLState::~OpenGLState()
	{
		glDeleteVertexArrays(1, &particle_vao_id);
		glDeleteVertexArrays(1, &vao_id);
		glDeleteVertexArrays(1, &font_vao_id);

		glDeleteBuffers(1, &vbo_font_id);
		glDeleteBuffers(1, &object_vbo_id);
		glDeleteBuffers(1, &normal_vbo_id);
	}

	void OpenGLState::GenResources()
	{
		auto& renderer_vertex_shaders = sys->renderer_vertex_shaders;
		auto& renderer_fragment_shaders = sys->renderer_fragment_shaders;

		renderer_vertex_shaders[VDebug]       = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/debug.vert");
		renderer_vertex_shaders[VNormalMesh]  = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/mesh.vert");
		renderer_vertex_shaders[VSkinnedMesh] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skinned_mesh.vert");
        renderer_vertex_shaders[VParticle] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/particle.vert");
		renderer_vertex_shaders[VSkyBox]      = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/skybox.vert");

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

		//fb_man.cBufferPickingTexture = Core::GetResourceManager().Create<OpenGLTexture>();
		//fb_man.cBufferPickingTexture->Bind();
		//auto pickMeta = fb_man.cBufferPickingTexture->GetMeta();
		//pickMeta.internal_format = ColorFormat::RUI_32;
		//pickMeta.format = InputChannels::RGBA;
		//fb_man.cBufferPickingTexture->SetMeta(pickMeta);
		////fb_man.cBufferPickingTexture->Buffer(nullptr, main_buffer->GetMeta().size, pickMeta.format, pickMeta.internal_format);
		//fb_man.cBufferPickingTexture->Size(ivec2{ 512 });

		FrameBufferBuilder builder;
		builder.Begin(ivec2{ 512,512 });
		builder.AddAttachment(
			AttachmentInfo
			{
				LoadOp::eClear,
				StoreOp::eStore,
				idk::ColorFormat::RGBF_32,
				FilterMode::_enum::Linear
			}
		);
		builder.SetDepthAttachment(
			AttachmentInfo
			{
				LoadOp::eClear,
				StoreOp::eStore,
				idk::ColorFormat::DEPTH_COMPONENT,
				FilterMode::_enum::Linear
			}
		);

		fb_man.pickingBuffer = { Core::GetResourceManager().GetFactory<FrameBufferFactory>().Create(builder.End()).guid };
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
		auto& renderer_vertex_shaders = sys->renderer_vertex_shaders;
		auto& renderer_fragment_shaders = sys->renderer_fragment_shaders;
		auto& font_render_data = curr_object_buffer.font_render_data;

		for (auto& cam : curr_object_buffer.camera)
		{
			auto& rt = *cam.render_target;
			if (rt.NeedsFinalizing())
				rt.Finalize();
		}


		assert(Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FBrdf]);
		ComputeBRDF(RscHandle<Program>{Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FBrdf]});

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
			pipeline.SetUniform("BoneMat4s.bone_transform", skeleton.bones_transforms);
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
                    if (!obj)
                        return;
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

		/*static const mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);*/

		for (const auto& elem : curr_object_buffer.lights)
		{
			pipeline.Use();
			if (elem.index == 0) // point light
				Core::GetSystem<DebugRenderer>().Draw(sphere{ elem.v_pos, 0.1f }, elem.light_color);

			else if (elem.index == 1) // directional light
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

				BindVertexShader(renderer_vertex_shaders[VertexShaders::VNormalMesh], light_p_tfm, light_view_tfm);

				for (auto& render_obj : curr_object_buffer.mesh_render)
				{
					pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FShadow]);
					PushMesh(RscHandle<OpenGLMesh>{render_obj.mesh});
					object_transforms.push_back(light_view_tfm * render_obj.transform);
				}
				FlushObjectTransforms();

				BindVertexShader(renderer_vertex_shaders[VertexShaders::VSkinnedMesh], light_p_tfm, light_view_tfm);
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

			else if (elem.index == 2) // spot light
				Core::GetSystem<DebugRenderer>().Draw(ray{ elem.v_pos, elem.v_dir }, elem.light_color);
		}

		glEnable(GL_DEPTH_TEST);
		RscHandle<FrameBuffer> main_buffer;

		auto& inst_buffer = curr_object_buffer.inst_mesh_render_buffer;
		UpdateInstancedBuffers(std::data(inst_buffer), inst_buffer.size() * sizeof(inst_buffer[0]));

		// range over cameras
		for(auto range: curr_object_buffer.culled_render_range)
		{
			auto& cam = range.camera;
			const auto inv_view_tfm = invert_rotation(cam.view_matrix);
			// calculate lights for this camera
			auto lights = curr_object_buffer.lights;
			curr_lights = span<LightData>{ lights };
			for (auto& elem : lights)
			{
				elem.v_pos = vec3{ cam.view_matrix * vec4{ elem.v_pos, 1 } };
				elem.v_dir = vec3{ cam.view_matrix * vec4{ elem.v_dir , 0 } };
			}

			{
				fb_man.SetRenderTarget(RscHandle<OpenGLRenderTarget>{cam.render_target},cam.viewport);

				// clear
				std::visit([&]([[maybe_unused]] const auto& obj)
				{
					static_assert(idk::is_variant_member_v <color, decltype(CameraData::clear_data)>, "Expected color in variant");
					using T = std::decay_t<decltype(obj)>;

					glEnable(GL_SCISSOR_TEST);

					if constexpr (std::is_same_v<T, RscHandle<CubeMap>>)
					{
						if (!obj)
							return;
						auto& oglCubeMap = std::get<RscHandle<CubeMap>>(cam.clear_data).as<OpenGLCubemap>();
						glClear(GL_DEPTH_BUFFER_BIT);

						pipeline.PushProgram(renderer_vertex_shaders[VSkyBox]);
						pipeline.PushProgram(renderer_fragment_shaders[FSkyBox]);
						pipeline.SetUniform("PerCamera.pv_transform", cam.projection_matrix * mat4(mat3(cam.view_matrix)));


						glDisable(GL_CULL_FACE);
						glDepthMask(GL_FALSE);
						oglCubeMap.BindToUnit(0);
						pipeline.SetUniform("sb", 0);
						RscHandle<OpenGLMesh>{*cam.CubeMapMesh}->BindAndDraw(renderer_attributes
							{ {
								std::make_pair(vtx::Attrib::Position, 0)
							} });
						glDepthMask(GL_TRUE);
						glEnable(GL_CULL_FACE);
					}

					if constexpr (std::is_same_v<T, color>)
					{
						glClearColor(obj.r, obj.g, obj.b, obj.a);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					}

					if constexpr (std::is_same_v<T, DepthOnly>)
					{
						glClear(GL_DEPTH_BUFFER_BIT);
					}

					glDisable(GL_SCISSOR_TEST);
				}, cam.clear_data);
			}
			curr_cam = cam;

			// lock drawing buffer
			pipeline.Use();
			pipeline.PopAllPrograms();
			glBindVertexArray(vao_id);
			
				
			

			BindVertexShader(renderer_vertex_shaders[VertexShaders::VDebug], cam.projection_matrix, cam.view_matrix);
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
			BindVertexShader(renderer_vertex_shaders[VertexShaders::VNormalMesh], cam.projection_matrix, cam.view_matrix);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			// If we only use proj_matrix, frustrum will be in view space.
			// If we use proj * view, frustrum will be in model space
			auto frust = camera_vp_to_frustum(cam.projection_matrix * cam.view_matrix);

			curr_mat = {};
			curr_mat_inst = {};
			curr_mesh = {};
			material_texture_uniforms = 0;
			{
				auto ptr = std::data(curr_object_buffer.instanced_mesh_render);
				if (range.inst_mesh_render_begin != range.inst_mesh_render_end)
				{

					for (auto itr = ptr + range.inst_mesh_render_begin,
						end = ptr + range.inst_mesh_render_end
						; itr != end; ++itr)
					{
						auto& elem = *itr;
						auto mesh = RscHandle<OpenGLMesh>{ elem.mesh };
						BindMaterialInstance(elem.material_instance);	   //todo check for superfluous binds.
						mesh->Bind(MeshRenderer::GetRequiredAttributes()); //todo check for superfluous binds.
						//if(!pipeline.SetUniform("PerCamera.perspective_transform", cam.projection_matrix))
						//	throw;
						UseInstancedBuffers();
						mesh->DrawInstanced(elem.instanced_index, elem.num_instances);
					}
				}
			}
			//
			//for (auto& elem : curr_object_buffer.mesh_render)
			//{
			//	// Do culling here
			//	sphere transformed_bounds = elem.mesh->bounding_volume * elem.transform;
			//
			//	// We only draw if the frustrum contains the mesh
			//	if (!frust.contains(transformed_bounds))
			//		continue;
			//
			//	/*
			//	// bind shader
			//	const auto material = elem.material_instance->material;
			//	pipeline.PushProgram(material->_shader_program);
			//
			//	// set probe
			//	GLuint texture_units = 0;
			//	//SetPBRUniforms     (cam, inv_view_tfm, texture_units);
			//	//SetLightUniforms   (span<LightData>{lights}, texture_units);
			//	//SetMaterialUniforms(elem.material_instance, texture_units);
			//	//SetObjectUniforms  (elem, cam.view_matrix);
			//	//
			//	//RscHandle<OpenGLMesh>{elem.mesh}->BindAndDraw<MeshRenderer>();
			//	*/
			//	PushMaterialInstance(elem.material_instance);
			//	PushMesh(RscHandle<OpenGLMesh>{elem.mesh});
			//	PushObjectTransform(elem.transform);
			//}
			//
			//FlushObjectTransforms();

			curr_mat = {};
			curr_mat_inst = {};
			curr_mesh = {};
			material_texture_uniforms = 0;

			BindVertexShader(renderer_vertex_shaders[VertexShaders::VSkinnedMesh], cam.projection_matrix, cam.view_matrix);
			for (auto& elem : curr_object_buffer.skinned_mesh_render)
			{
				// Do culling here
				//sphere transformed_bounds = elem.mesh->bounding_volume * elem.transform;

				// We only draw if the frustrum contains the mesh
				//if (!frust.contains(transformed_bounds))
				//	continue;

				// bind shader
				const auto material = elem.material_instance->material;
				pipeline.PushProgram(material->_shader_program);

				//PushMaterialInstance(elem.material_instance);
				//SetSkeletons(elem.skeleton_index);
				//PushMesh(RscHandle<OpenGLMesh>{elem.mesh});
				//PushObjectTransform(elem.transform);

				GLuint texture_units = 0;
				SetPBRUniforms     (cam, inv_view_tfm, texture_units);
				SetLightUniforms   (span<LightData>{lights}, texture_units);
				SetSkeletons(elem.skeleton_index);
				SetMaterialUniforms(elem.material_instance, texture_units);
				SetObjectUniforms  (elem, cam.view_matrix);

				RscHandle<OpenGLMesh>{elem.mesh}->BindAndDraw<SkinnedMeshRenderer>();
			}

			//FlushObjectTransforms();

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

			/* Set up the VBO for our vertex data */
			glBindVertexArray(font_vao_id);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			////////////////////////FONT///////////////////////
			pipeline.PushProgram(renderer_vertex_shaders[VFont]);
			pipeline.PushProgram(renderer_fragment_shaders[FFont]);

			pipeline.SetUniform("PerCamera.perspective_transform", cam.projection_matrix);
			//glDisable(GL_CULL_FACE);
			for (auto& elem : font_render_data)
			{
				if (elem.coords.size())
				{
					auto& atlas = elem.fontAtlas.as<OpenGLFontAtlas>();

					glBindBuffer(GL_ARRAY_BUFFER, vbo_font_id);
					/* Use the texture containing the atlas */
					atlas.BindToUnit(0);
					pipeline.SetUniform("tex", 0);

					SetObjectUniforms(elem, cam.view_matrix);
					pipeline.SetUniform("PerFont.color", elem.color.as_vec4);
					GL_CHECK();

					//pipeline.SetUniform("ColorBlk.color", elem.color.as_vec3);

					/* Draw all the character on the screen in one go */
				
					glBufferData(GL_ARRAY_BUFFER, elem.coords.size() * sizeof(FontPoint), std::data(elem.coords), GL_DYNAMIC_DRAW);
					//GL_CHECK();
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(real), 0);
					//GL_CHECK();
					glDrawArrays(GL_TRIANGLES, 0, elem.n_size);

					glDisableVertexAttribArray(0);

					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
				GL_CHECK();
			}
			//glEnable(GL_CULL_FACE);

			glBindVertexArray(0);

            static vector<OpenGLBuffer> bufs = []()
            {
                vector<OpenGLBuffer> bufs;
                unsigned int indices[]{ 0, 3, 1, 1, 3, 2 };
                bufs.emplace_back(OpenGLBuffer{ GL_ELEMENT_ARRAY_BUFFER, {} })
                    .Bind().Buffer(indices, sizeof(int), 6);
                bufs.emplace_back(OpenGLBuffer{ GL_ARRAY_BUFFER, {
                    { vtx::Attrib::ParticlePosition, sizeof(ParticleObj), offsetof(ParticleObj, position) },
                    { vtx::Attrib::ParticleRotation, sizeof(ParticleObj), offsetof(ParticleObj, rotation) },
                    { vtx::Attrib::ParticleSize, sizeof(ParticleObj), offsetof(ParticleObj, size) },
                    { vtx::Attrib::Color, sizeof(ParticleObj), offsetof(ParticleObj, color) }
                } });
                return bufs;
            }();


            

			glBindVertexArray(particle_vao_id);
            BindVertexShader(renderer_vertex_shaders[VertexShaders::VParticle], cam.projection_matrix, cam.view_matrix);
            vec3 cam_forward{ -cam.view_matrix[0][2], -cam.view_matrix[1][2], -cam.view_matrix[2][2] };
            vec3 cam_pos = cam.view_matrix[3];
            auto particle_render_data = curr_object_buffer.particle_render_data;
            for (auto& elem : particle_render_data)
            {
                std::sort(elem.particles.begin(), elem.particles.end(),
                    [cam_forward, cam_pos](const ParticleObj& a, const ParticleObj& b) { return (a.position - cam_pos).dot(cam_forward) > (b.position - cam_pos).dot(cam_forward); });

                // bind shader
                const auto material = elem.material_instance->material;
                pipeline.PushProgram(material->_shader_program);
                GLuint texture_units = 0;
                SetMaterialUniforms(elem.material_instance, texture_units);

                RscHandle<OpenGLMesh>{Mesh::defaults[MeshType::FSQ]}->Bind(
                    renderer_attributes{ {
                        { vtx::Attrib::Position, 0 },
                        { vtx::Attrib::UV, 1 },
                    }
                });
                glVertexAttribDivisor(0, 0);
                glVertexAttribDivisor(1, 0);

                bufs[1].Bind().Buffer(elem.particles.data(), sizeof(ParticleObj), static_cast<GLsizei>(elem.particles.size()));
                bufs[1].BindForDraw(renderer_attributes{ {
                    {vtx::Attrib::ParticlePosition, 2},
                    {vtx::Attrib::ParticleRotation, 3},
                    {vtx::Attrib::ParticleSize, 4},
                    {vtx::Attrib::Color, 5}
                } });
                glVertexAttribDivisor(2, 1);
                glVertexAttribDivisor(3, 1);
                glVertexAttribDivisor(4, 1);
                glVertexAttribDivisor(5, 1);

                bufs[0].Bind(); // index buffer
                glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, static_cast<GLsizei>(elem.particles.size()));
            }

			glDisable(GL_BLEND);
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		glBindVertexArray(0);

		fb_man.ResetFramebuffer();



		//////////////////////////////////For PICKING/////////////////////////////////
		// set main scene camera
		
		//vector<OpenGLTexture> addMainBuffer;
		//addMainBuffer.emplace_back(cBufferPickingTexture);
		if(0)
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
					fb_man.SetRenderTarget(fb_man.pickingBuffer);
				}

				// lock drawing buffer
				pipeline.Use();
				pipeline.PopAllPrograms();
				glBindVertexArray(vao_id);

				BindVertexShader(renderer_vertex_shaders[VertexShaders::VDebug], cam.projection_matrix, cam.view_matrix);
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
				BindVertexShader(renderer_vertex_shaders[VertexShaders::VNormalMesh], cam.projection_matrix, cam.view_matrix);
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

				BindVertexShader(renderer_vertex_shaders[VertexShaders::VSkinnedMesh], cam.projection_matrix, cam.view_matrix);
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

		pipeline.PushProgram(Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VPBRConvolute]);
		pipeline.PushProgram(Core::GetSystem<GraphicsSystem>().renderer_geometry_shaders[GSinglePassCube]);
		pipeline.PushProgram(Core::GetSystem<GraphicsSystem>().renderer_fragment_shaders[FPBRConvolute]);

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

		pipeline.PushProgram(Core::GetSystem<GraphicsSystem>().renderer_vertex_shaders[VFsq]);
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
	}

	PixelData OpenGLState::PickData(const vec2& pos)
	{
		PixelData pd;
		glReadPixels((GLint)pos.x, (GLint)pos.y, fb_man.pickingBuffer->size.x, fb_man.pickingBuffer->size.y, GL_RGB32F, GL_UNSIGNED_BYTE, &pd);

		return std::move(pd);
	}

	void OpenGLState::IsPicking()
	{
		is_picking = true;
	}

	void OpenGLState::PushMaterial(const RscHandle<Material>& mat)
	{
		if (curr_mat != mat)
		{
			FlushObjectTransforms();

			pipeline.PushProgram(mat->_shader_program);
			curr_mat = mat;
			curr_mat_inst = {};

			material_texture_uniforms = 0;

			auto inv_view_tfm = curr_cam.view_matrix.inverse();

			// bind pbr uniforms
			std::visit([&]([[maybe_unused]] const auto& obj)
				{
					using T = std::decay_t<decltype(obj)>;
					if constexpr (std::is_same_v<T, RscHandle<CubeMap>>)
					{
						if (!obj)
							return;
						const auto opengl_handle = RscHandle<ogl::OpenGLCubemap>{ obj };
						opengl_handle->BindConvolutedToUnit(material_texture_uniforms);
						pipeline.SetUniform("irradiance_probe", material_texture_uniforms++);
						opengl_handle->BindToUnit(material_texture_uniforms);
						pipeline.SetUniform("environment_probe", material_texture_uniforms++);
					}
				}, curr_cam.clear_data);

			brdf_texture->BindToUnit(material_texture_uniforms);
			pipeline.SetUniform("brdfLUT", material_texture_uniforms++);
			pipeline.SetUniform("PerCamera.inverse_view_transform", inv_view_tfm);

			pipeline.SetUniform("LightBlk.light_count", (int)curr_lights.size());

			for (unsigned i = 0; i < curr_lights.size(); ++i)
			{
				auto& light = curr_lights[i];
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
					t.as<OpenGLTexture>().BindToUnit(material_texture_uniforms);

					pipeline.SetUniform(lightblk + "vp", light.vp);
					(pipeline.SetUniform("shadow_maps[" + std::to_string(i) + "]", material_texture_uniforms));
					material_texture_uniforms++;
				}
			}
		}
	}

	void OpenGLState::BindMaterialInstance(const RscHandle<MaterialInstance>& mat_inst)
	{
		PushMaterial(mat_inst->material);

		auto instance_texture_units = material_texture_uniforms;
		for (auto& [name, uniform] : mat_inst->material->uniforms)
		{
			auto val = *mat_inst->GetUniform(name);
			std::visit([this, &instance_texture_units, id = uniform.name](auto& elem)
			{
				using T = std::decay_t<decltype(elem)>;
				if constexpr (std::is_same_v<T, RscHandle<Texture>>)
				{
					const auto texture = RscHandle<ogl::OpenGLTexture>{ elem };
					texture->BindToUnit(instance_texture_units);
					pipeline.SetUniform(id, instance_texture_units);

					++instance_texture_units;
				}
				else
					pipeline.SetUniform(id, elem);
			}, val);
		}
		for (auto& uniform : mat_inst->material->hidden_uniforms)
		{
			std::visit([this, &instance_texture_units, id = uniform.name](auto& elem)
			{
				using T = std::decay_t<decltype(elem)>;
				if constexpr (std::is_same_v<T, RscHandle<Texture>>)
				{
					const auto texture = RscHandle<ogl::OpenGLTexture>{ elem };
					texture->BindToUnit(instance_texture_units);
					pipeline.SetUniform(id, instance_texture_units);

					++instance_texture_units;
				}
				else
					pipeline.SetUniform(id, elem);
			}, uniform.value);
		}

		curr_mat_inst = mat_inst;
	}

	void OpenGLState::PushMaterialInstance(const RscHandle<MaterialInstance>& mat_inst)
	{
		if (curr_mat_inst != mat_inst)
		{
			FlushObjectTransforms();
			BindMaterialInstance(mat_inst);
		}
	}

	void OpenGLState::PushMesh(const RscHandle<OpenGLMesh>& mesh)
	{
		if (curr_mesh != mesh)
		{
			FlushObjectTransforms();

			mesh->Bind(MeshRenderer::GetRequiredAttributes());
			curr_mesh = mesh;
		}
	}

	void OpenGLState::PushObjectTransform(const mat4& tfm)
	{
		object_transforms.emplace_back(curr_cam.view_matrix * tfm);
	}
	namespace detail
	{

		void UpdateInstancedBuffer(unsigned loc,GLuint normal_vbo_id, const void* buffer,size_t initial_offset, size_t num_bytes, size_t stride)
		{
			constexpr auto BindMat = [](unsigned loc,size_t stride,size_t initial_offset)
			{
				for (unsigned i = 0; i < 4; ++i)
				{
					glEnableVertexAttribArray(loc + i);
					glVertexAttribPointer(loc + i, 4, GL_FLOAT, GL_FALSE, stride, (void*)(initial_offset + sizeof(vec4) * i));
					glVertexAttribDivisor(loc + i, 1);
				}
			};

			// bind to vertex loc

			glBindBuffer(GL_ARRAY_BUFFER, normal_vbo_id);
			glBufferData(GL_ARRAY_BUFFER, num_bytes, buffer, GL_DYNAMIC_DRAW);
			//BindMat(loc, stride, initial_offset);
		}

		void UpdateInstancedBuffers(unsigned mv_loc, unsigned nrm_loc,GLuint object_vbo_id, GLuint normal_vbo_id, const void* buffer, size_t num_bytes, size_t stride)
		{
			UpdateInstancedBuffer(mv_loc , object_vbo_id, buffer, 0           ,num_bytes, stride);
			UpdateInstancedBuffer(nrm_loc, normal_vbo_id, buffer, sizeof(mat4),num_bytes, stride);
		}
		void UseInstancedBuffer(GLuint object_vbo_id, GLuint normal_vbo_id)
		{
			glBindBuffer(GL_ARRAY_BUFFER, object_vbo_id);
			glBindBuffer(GL_ARRAY_BUFFER, normal_vbo_id);
		}


	}

	void OpenGLState::FlushObjectTransforms()
	{
		normal_transforms.clear();
		normal_transforms.reserve(object_transforms.size());
		std::transform(object_transforms.begin(), object_transforms.end(), std::back_inserter(normal_transforms),
			[](const mat4& obj_tfm) -> mat4
			{
				return obj_tfm.inverse().transpose();
			});

		constexpr auto BindMat = [](unsigned loc)
		{
			for (unsigned i = 0; i < 4; ++i)
			{
				glEnableVertexAttribArray(loc + i);
				glVertexAttribPointer(loc + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*) (sizeof(vec4) * i));
				glVertexAttribDivisor(loc + i, 1);
			}
		};

		glBindBuffer(GL_ARRAY_BUFFER, object_vbo_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * object_transforms.size(), std::data(object_transforms), GL_DYNAMIC_DRAW);
		BindMat(4);
		// bind to vertex loc

		glBindBuffer(GL_ARRAY_BUFFER, normal_vbo_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * normal_transforms.size(), std::data(normal_transforms), GL_DYNAMIC_DRAW);
		BindMat(8);

		curr_mesh->DrawInstanced(object_transforms.size());
		object_transforms.clear();
	}
	void OpenGLState::UseInstancedBuffers()
	{
		constexpr auto BindMat = [](unsigned loc,size_t offset=0)
		{
			for (unsigned i = 0; i < 4; ++i)
			{
				glEnableVertexAttribArray(loc + i);
				glVertexAttribPointer(loc + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4)*2, (void*)(offset + sizeof(vec4) * i));
				glVertexAttribDivisor(loc + i, 1);
			}
		};
		glBindBuffer(GL_ARRAY_BUFFER, object_vbo_id);
		BindMat(4,0);
		glBindBuffer(GL_ARRAY_BUFFER, normal_vbo_id);
		BindMat(8,sizeof(mat4));
	}

	void OpenGLState::UpdateInstancedBuffers(const void* data, size_t num_bytes)
	{
		detail::UpdateInstancedBuffers(4, 8, object_vbo_id, normal_vbo_id, data, num_bytes, sizeof(mat4) * 2);
		num_inst_bytes = num_bytes;
	}

}
