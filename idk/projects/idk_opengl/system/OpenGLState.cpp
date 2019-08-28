#include "pch.h"
#include <sstream>

#include <gfx/MeshRenderer.h>

#include <idk_opengl/resource/OpenGLMesh.h>
#include <idk_opengl/resource/OpenGLMaterial.h>

#include <core/Core.h>
#include <file/FileSystem.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include "OpenGLState.h"

namespace idk::ogl
{
	void OpenGLState::Setup()
	{
		sys = &Core::GetSystem<Win32GraphicsSystem>();
		glGenVertexArrays(1, &vao_id);
	}

	void OpenGLState::GenResources()
	{
		// generate mesh renderer
		auto vtx_shader = Core::GetSystem<FileSystem>().Open("/assets/shader/mesh.vert", FS_PERMISSIONS::READ, false);
		std::stringstream stringify;
		stringify << vtx_shader.rdbuf();

		renderer_vertex_shaders.emplace_back(RendererInfo{ reflect::typehash<MeshRenderer>(),
			std::move(
				Program{}
					.Attach(Shader{GL_VERTEX_SHADER,
						stringify.str()
						})
					.Link()
			) }
		);
	}


	void OpenGLState::RenderDrawBuffer()
	{
		auto& object_buffer = sys->object_buffer;
		auto& curr_write_buffer = sys->curr_write_buffer;
		auto& curr_draw_buffer = sys->curr_draw_buffer;
		// lock drawing buffer
		curr_draw_buffer = curr_write_buffer;
		
		// render mesh renderers
		auto itr_to_mesh_vtx = std::find_if(renderer_vertex_shaders.begin(), renderer_vertex_shaders.end(),
			[](const auto& elem)->bool {
				return elem.typehash == reflect::typehash<MeshRenderer>();
			});

		pipeline.Use();
		pipeline.PushProgram(itr_to_mesh_vtx->vertex_shader);

		glBindVertexArray(vao_id);
		for (auto& elem : object_buffer[curr_draw_buffer].mesh_render)
		{
			// bind shader
			auto& material = elem.material_instance.material.as<OpenGLMaterial>();
			pipeline.PushProgram(material.GetShaderProgram());

			// bind attribs
			auto& mesh = elem.mesh.as<OpenGLMesh>();
			mesh.Bind(MeshRenderer::GetRequiredAttributes());

			// set uniforms
			// object uniforms
			pipeline.SetUniform("object_transform", elem.transform);
			pipeline.SetUniform("normal_transform", elem.transform.inverse().transpose());

			// material uniforms
			for (auto& [id, uniform] : elem.material_instance.uniforms)
			{
				std::visit([this, &id](auto& elem) {
					pipeline.SetUniform(id, elem);
				}, uniform);
			}

			// draw
			mesh.Draw();
		}
	}

}
