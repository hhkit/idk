#include "pch.h"
#include <ds/span.inl>
#include <ds/result.inl>
#include <res/ResourceHandle.inl>

#include <gfx/DebugRenderer.h>
#include <gfx/MeshRenderer.h>

#include <opengl/resource/OpenGLMesh.h>
#include <opengl/resource/OpenGLTexture.h>
#include <opengl/resource/FrameBuffer.h>
#include <gfx/FrameBuffer.h>

#include <core/Core.h>
#include <file/FileSystem.h>
#include <math/matrix_transforms.inl>
#include <res/ResourceManager.inl>
#include <res/ResourceHandle.inl>
#include <opengl/system/OpenGLGraphicsSystem.h>
#include "OpenGLState.h"
#include <opengl/program/PipelineProgram.inl>
#include <opengl/program/Program.inl>
#include <anim/SkinnedMeshRenderer.h>
#include <gfx/CubeMap.h>
#include <opengl/resource/OpenGLCubemap.h>
#include <math/shapes/frustum.h>
#include <gfx/FramebufferFactory.h>
#include <gfx/FontAtlas.h>
#include <opengl/resource/OpenGLFontAtlas.h>
#include <ui/Canvas.h>

#include <editor/IDE.h>
#include <gfx/ViewportUtil.h>
#include <res/ResourceMeta.inl>

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

		brdf_texture = Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(TextureInternalFormat::RG_16_F, uvec2{512});
		fb_man.cBufferPickingTexture = Core::GetResourceManager().LoaderEmplaceResource<OpenGLTexture>(TextureInternalFormat::R_32_UI, uvec2{ 512 });

		FrameBufferBuilder builder;
		builder.Begin("Picking Buffer", uvec2{ 512,512 });
		builder.AddAttachment(
			AttachmentInfo
			{
				LoadOp::eClear,
				StoreOp::eStore,
				TextureInternalFormat::RGB_32_F,
				FilterMode::Linear
			}
		);
		builder.SetDepthAttachment(
			AttachmentInfo
			{
				LoadOp::eClear,
				StoreOp::eStore,
				DepthBufferMode::Depth16,
				false,
				FilterMode::Linear
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

		if (RscHandle<RenderTarget>{}->NeedsFinalizing())
			RscHandle<RenderTarget>{}->Finalize();
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

		const auto SetLightUniforms = [this](span<LightData> lights, [[maybe_unused]] GLuint& texture_units)
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
				pipeline.SetUniform(lightblk + "falloff", light.falloff);


				//SHADOW IS COMMENTED
				/*if (light.light_map)
				{
					const auto t = light.light_map->DepthAttachment().buffer;
					t.as<OpenGLTexture>().BindToUnit(texture_units);

					pipeline.SetUniform(lightblk + "vp", light.vp);
					pipeline.SetUniform("shadow_maps[" + std::to_string(i) + "]", texture_units);
					texture_units++;
				}*/
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
			{
				Core::GetSystem<DebugRenderer>().Draw(sphere{ elem.v_pos, 0.1f }, elem.light_color);
				//fb_man.SetRenderTarget(s_cast<RscHandle<OpenGLFrameBuffer>>(elem.light_map));

				//glClearColor(1.f, 1.f, 1.f, 1.f);
				//glClearDepth(1.f);
				//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				//glEnable(GL_DEPTH_TEST);
				//glDepthFunc(GL_LESS);

				//glCullFace(GL_FRONT); //Fix for peterpanning

				////const auto& light_view_tfm = elem.v;

				////////////////////////////////////Calculate light view transform//////////////////////////////////////
				//vector<mat4> matList;
				//matList.emplace_back(look_at(elem.v_pos, elem.v_pos - vec3(1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f)).inverse());
				//matList.emplace_back(look_at(elem.v_pos, elem.v_pos - vec3(-1.f, 0.f, 0.f), vec3(0.f, -1.f, 0.f)).inverse());
				//matList.emplace_back(look_at(elem.v_pos, elem.v_pos - vec3(0.f, 1.f, 0.f), vec3(0.f, 0.f, 1.f)).inverse());
				//matList.emplace_back(look_at(elem.v_pos, elem.v_pos - vec3(0.f, -1.f, 0.f), vec3(0.f, 0.f, -1.f)).inverse());
				//matList.emplace_back(look_at(elem.v_pos, elem.v_pos - vec3(0.f, 0.f, 1.f), vec3(0.f, -1.f, 0.f)).inverse());
				//matList.emplace_back(look_at(elem.v_pos, elem.v_pos - vec3(0.f, 0.f, -1.f), vec3(0.f, -1.f, 0.f)).inverse());


				////////////WIP////////


				////auto& size = s_cast<RscHandle<OpenGLFrameBuffer>>(elem.light_map)->size;
				////const auto& light_p_tfm = elem.p;

				//pipeline.PopAllPrograms();

				//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				//pipeline.PushProgram(renderer_geometry_shaders[GSinglePassCube]);

				//BindVertexShader(renderer_vertex_shaders[VertexShaders::VNormalMesh], light_p_tfm, light_view_tfm);

			}
			else if (elem.index == 1) // directional light
			{
				Core::GetSystem<DebugRenderer>().Draw(ray{ elem.v_pos, elem.v_dir * 0.25f }, elem.light_color);
				
				//YOU HAVE NO SHADOW
				//fb_man.SetRenderTarget(s_cast<RscHandle<OpenGLFrameBuffer>>(elem.light_map));

				glClearColor(1.f,1.f,1.f,1.f);
				glClearDepth(1.f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				glEnable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);

				glCullFace(GL_FRONT); //Fix for peterpanning

				const auto& light_view_tfm = elem.v;
				const auto& light_p_tfm = elem.p;
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
			if (cam.render_target->RenderDebug() && cam.render_target->IsWorldRenderer())
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

			auto c_mat = curr_mat = {};
			auto c_mat_inst = curr_mat_inst = {};
			auto c_mesh = curr_mesh = {};
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
						if (c_mat_inst != elem.material_instance)
						{
							c_mat_inst = elem.material_instance;
							if (c_mat != c_mat_inst->material)
							{
								c_mat = c_mat_inst->material;
								BindMaterial(c_mat);
								pipeline.SetUniform("PerCamera.perspective_transform", cam.projection_matrix);
							}
							BindMaterialInstance(c_mat_inst);	   

						}
						if (c_mesh != mesh)
						{
							c_mesh = mesh;
							mesh->Bind(MeshRenderer::GetRequiredAttributes());
						}
						UseInstancedBuffers(elem.instanced_index);
						mesh->DrawInstanced(elem.num_instances);
					}
				}
			}

			curr_mat = {};
			curr_mat_inst = {};
			curr_mesh = {};
			material_texture_uniforms = 0;

			BindVertexShader(renderer_vertex_shaders[VertexShaders::VSkinnedMesh], cam.projection_matrix, cam.view_matrix);
			for (auto& elem : curr_object_buffer.skinned_mesh_render)
			{
				SetSkeletons(elem.skeleton_index);
				// bind shader
				const auto material = elem.material_instance->material;
				if (curr_mat != material)
				{
					pipeline.PushProgram(material->_shader_program);

					// material 
					material_texture_uniforms = 0;
					SetPBRUniforms(cam, inv_view_tfm, material_texture_uniforms);
					SetLightUniforms(span<LightData>{lights}, material_texture_uniforms);
					curr_mat = material;
					curr_mat_inst = {};
				}
				// material instance
				if (curr_mat_inst != elem.material_instance)
				{
					SetMaterialUniforms(elem.material_instance, material_texture_uniforms);
					curr_mat_inst = elem.material_instance;
				}
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
					auto& atlas = elem.atlas.as<OpenGLFontAtlas>();

					glBindBuffer(GL_ARRAY_BUFFER, vbo_font_id);
					/* Use the texture containing the atlas */
					atlas.BindToUnit(0);
					pipeline.SetUniform("tex", 0);

					SetObjectUniforms(elem, cam.view_matrix);
					pipeline.SetUniform("PerFont.color", elem.color.as_vec4);

					/* Draw all the character on the screen in one go */
				
					glBufferData(GL_ARRAY_BUFFER, elem.coords.size() * sizeof(FontPoint), std::data(elem.coords), GL_DYNAMIC_DRAW);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(real), 0);
					glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(elem.coords.size()));
					glDisableVertexAttribArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, 0);
				}
			}

			glBindVertexArray(0);



            // PARTICLE SYSTEM DRAW

			glBindVertexArray(particle_vao_id);
            BindVertexShader(renderer_vertex_shaders[VertexShaders::VParticle], cam.projection_matrix, cam.view_matrix);
            const vec3 cam_forward{ -cam.view_matrix[0][2], -cam.view_matrix[1][2], -cam.view_matrix[2][2] };
            const vec3 cam_pos = cam.view_matrix[3];
            auto particle_render_data = curr_object_buffer.particle_render_data;

            const RscHandle<OpenGLMesh> fsq{ Mesh::defaults[MeshType::FSQ] };

            for (auto& elem : particle_render_data)
            {
                std::sort(elem.particles.begin(), elem.particles.end(),
                    [cam_forward, cam_pos](const ParticleObj& a, const ParticleObj& b) {
                        return (a.position - cam_pos).dot(cam_forward) > (b.position - cam_pos).dot(cam_forward); });

                // bind shader
                const auto material = elem.material_instance->material;
                pipeline.PushProgram(material->_shader_program);
                GLuint texture_units = 0;
                SetMaterialUniforms(elem.material_instance, texture_units);

                fsq->Bind(
                    renderer_attributes{ {
                        { vtx::Attrib::Position, 0 },
                        { vtx::Attrib::UV, 1 },
                    }
                });
                glVertexAttribDivisor(0, 0);
                glVertexAttribDivisor(1, 0);

                particle_buf.BindForDraw(renderer_attributes{ {
                    {vtx::Attrib::ParticlePosition, 2},
                    {vtx::Attrib::ParticleRotation, 3},
                    {vtx::Attrib::ParticleSize, 4},
                    {vtx::Attrib::Color, 5}
                } });
                particle_buf.Buffer(elem.particles.data(), sizeof(ParticleObj), static_cast<GLsizei>(elem.particles.size()));

                glVertexAttribDivisor(2, 1);
                glVertexAttribDivisor(3, 1);
                glVertexAttribDivisor(4, 1);
                glVertexAttribDivisor(5, 1);

                fsq->DrawInstanced(elem.particles.size());
            }

		} // for each culled camera

		glBindVertexArray(0);



        // UI DRAW
        glDisable(GL_DEPTH_TEST);

        RscHandle<OpenGLMesh> fsq{ Mesh::defaults[MeshType::FSQ] };
        glBindVertexArray(font_vao_id);
        pipeline.PushProgram(renderer_vertex_shaders[VertexShaders::VUi]);
        auto ui_render_per_canvas = curr_object_buffer.ui_render_per_canvas;
        for (auto& [canvas, ui_render_data] : ui_render_per_canvas)
        {
			//if (canvas->render_target)
			{
				fb_man.SetRenderTarget(RscHandle<OpenGLRenderTarget>{canvas->render_target});
				for (auto& elem : ui_render_data)
				{
					std::visit([&](const auto& data)
					{
						using T = std::decay_t<decltype(data)>;
						if constexpr (std::is_same_v<T, ImageData>)
						{
							if (data.texture)
							{
								// bind shader
								pipeline.PushProgram(elem.material->material->_shader_program);
								pipeline.SetUniform("tex", RscHandle<ogl::OpenGLTexture>{ data.texture }, 0);
								pipeline.SetUniform("PerUI.color", vec4{ elem.color });
								pipeline.SetUniform("PerUI.is_font", false);
								pipeline.SetUniform("ObjectMat4s.object_transform", elem.transform);

								fsq->Bind(
									renderer_attributes{ {
										{ vtx::Attrib::UV, 1 },
										{ vtx::Attrib::Position, 0 }
									} }
								);
								glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(real), 0);

								fsq->Draw();
							}
						}
						else
						{
							if (data.coords.size())
							{
								pipeline.PushProgram(elem.material->material->_shader_program);

								auto& atlas = data.atlas.as<OpenGLFontAtlas>();

								glBindBuffer(GL_ARRAY_BUFFER, vbo_font_id);
								/* Use the texture containing the atlas */
								atlas.BindToUnit(0);
								pipeline.SetUniform("tex", 0);

								pipeline.SetUniform("PerUI.color", vec4{ elem.color });
								pipeline.SetUniform("PerUI.is_font", true);
								pipeline.SetUniform("ObjectMat4s.object_transform", elem.transform);

								/* Draw all the character on the screen in one go */

								glBufferData(GL_ARRAY_BUFFER, data.coords.size() * sizeof(FontPoint), std::data(data.coords), GL_DYNAMIC_DRAW);
								//GL_CHECK();
								glEnableVertexAttribArray(0);
								glEnableVertexAttribArray(1);
								glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(real), 0); // pos
								glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(real), r_cast<void*>(2 * sizeof(real))); // uv
								//GL_CHECK();
								glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(data.coords.size()));

								glDisableVertexAttribArray(0);
								glDisableVertexAttribArray(1);

								glBindBuffer(GL_ARRAY_BUFFER, 0);
							}
						}
					}, elem.data);
				}
			}
        }

		fb_man.ResetFramebuffer();

		//////////////////////////////////For PICKING/////////////////////////////////
		// set main scene camera
		
		if(Core::GetSystem<GraphicsSystem>().enable_picking)
		{		
			auto cam = curr_object_buffer.camera[curr_object_buffer.curr_scene_camera_index];
			{
				{
					fb_man.SetRenderTarget(fb_man.pickingBuffer);
				}

				// lock drawing buffer
				pipeline.Use();
				pipeline.PopAllPrograms();
				glBindVertexArray(vao_id);

				BindVertexShader(renderer_vertex_shaders[VertexShaders::VDebug], cam.projection_matrix, cam.view_matrix);
				pipeline.PushProgram(renderer_fragment_shaders[FragmentShaders::FPicking]);

				// per mesh render
				BindVertexShader(renderer_vertex_shaders[VertexShaders::VNormalMeshPicker], cam.projection_matrix, cam.view_matrix);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				for (auto& elem : curr_object_buffer.mesh_render)
				{
					// bind shader
					const auto material = elem.material_instance->material;
					pipeline.PushProgram(material->_shader_program);

					// set probe
					//GLuint texture_units = 0;
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

					//GLuint texture_units = 0;
					SetSkeletons(elem.skeleton_index);
					SetObjectUniforms(elem, cam.view_matrix);

					pipeline.SetUniform("obj_index", elem.obj_id);

					RscHandle<OpenGLMesh>{elem.mesh}->BindAndDraw<SkinnedMeshRenderer>();
				}
			}

			fb_man.ResetFramebuffer();
		}
		
		if (&Core::GetSystem<IEditor>() == nullptr)
		{
			auto outbuf_sz = RscHandle<OpenGLRenderTarget>{}->size;
			auto screen_sz = Core::GetSystem<Application>().GetScreenSize();
			fb_man.SetRenderTarget(RscHandle<OpenGLRenderTarget>{});

			glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_man.ID());
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glViewport(0, 0, screen_sz.x, screen_sz.y);
			glBlitFramebuffer(0, 0, outbuf_sz.x, outbuf_sz.y, 0, 0, screen_sz.x, screen_sz.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

			glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
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
		fb_man.SetRenderTarget(fb_man.pickingBuffer);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fb_man.ID());
		GL_CHECK();
		glReadPixels((GLint)pos.x, (GLint)pos.y, fb_man.pickingBuffer->size.x, fb_man.pickingBuffer->size.y, GL_RGB32F, GL_UNSIGNED_BYTE, &pd);
		GL_CHECK();
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		GL_CHECK();
		return std::move(pd);
	}

	void OpenGLState::IsPicking()
	{
		is_picking = true;
	}

	void OpenGLState::LoadShaderImpl()
	{
		auto& renderer_vertex_shaders = sys->renderer_vertex_shaders;
		//auto& renderer_fragment_shaders = sys->renderer_fragment_shaders;
		auto& renderer_geometry_shaders = sys->renderer_geometry_shaders;
		renderer_geometry_shaders;

		// If your shader is used by both vulkan and opengl, put it in GraphicsSystem.cpp's LoadShaders
		renderer_vertex_shaders[VDebug] = *Core::GetResourceManager().Load<ShaderProgram>("/engine_data/shaders/debug.vert");

	}

	void OpenGLState::BindMaterial(const RscHandle<Material>& mat)
	{
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
			pipeline.SetUniform(lightblk + "falloff", light.falloff);

			/*if (light.light_map)
			{
				const auto t = light.light_map->DepthAttachment().buffer;
				t.as<OpenGLTexture>().BindToUnit(material_texture_uniforms);

				pipeline.SetUniform(lightblk + "vp", light.vp);
				(pipeline.SetUniform("shadow_maps[" + std::to_string(i) + "]", material_texture_uniforms));
				material_texture_uniforms++;
			}*/
		}
	}

	void OpenGLState::PushMaterial(const RscHandle<Material>& mat)
	{
		if (curr_mat != mat)
		{
			FlushObjectTransforms();
			BindMaterial(mat);
		}
	}

	void OpenGLState::BindMaterialInstance(const RscHandle<MaterialInstance>& mat_inst)
	{
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
			PushMaterial(mat_inst->material);
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

		void UpdateInstancedBuffer(GLuint normal_vbo_id, const void* buffer,size_t num_bytes)
		{
			// bind to vertex loc

			glBindBuffer(GL_ARRAY_BUFFER, normal_vbo_id);
			glBufferData(GL_ARRAY_BUFFER, num_bytes, buffer, GL_DYNAMIC_DRAW);
			//BindMat(loc, stride, initial_offset);
		}

		void UpdateInstancedBuffers(GLuint object_vbo_id, GLuint normal_vbo_id, const void* buffer, size_t num_bytes)
		{
			UpdateInstancedBuffer(object_vbo_id, buffer, num_bytes);
			UpdateInstancedBuffer(normal_vbo_id, buffer, num_bytes);
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
	void OpenGLState::UseInstancedBuffers(size_t starting_instance)
	{
		constexpr auto BindMat = [](unsigned loc,size_t starting_instance,size_t offset=0)
		{
			GLsizei stride = sizeof(mat4) * 2;
			for (unsigned i = 0; i < 4; ++i)
			{
				glEnableVertexAttribArray(loc + i);
				glVertexAttribPointer(loc + i, 4, GL_FLOAT, GL_FALSE, stride, (void*)(starting_instance*stride+offset + sizeof(vec4) * i));
				glVertexAttribDivisor(loc + i, 1);
			}
		};
		glBindBuffer(GL_ARRAY_BUFFER, object_vbo_id);
		BindMat(4, starting_instance,0);
		glBindBuffer(GL_ARRAY_BUFFER, normal_vbo_id);
		BindMat(8,starting_instance,sizeof(mat4));
	}

	void OpenGLState::UpdateInstancedBuffers(const void* data, size_t num_bytes)
	{
		detail::UpdateInstancedBuffers(object_vbo_id, normal_vbo_id, data, num_bytes);
		num_inst_bytes = num_bytes;
	}

}
