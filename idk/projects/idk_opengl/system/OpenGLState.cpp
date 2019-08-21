#include "pch.h"
#include "OpenGLState.h"
#include <gfx/MeshRenderer.h>

#include <idk_opengl/resource/OpenGLMesh.h>
#include <idk_opengl/resource/OpenGLMaterial.h>

namespace idk::ogl
{
	void OpenGLState::Setup()
	{
		// generate mesh renderer
		renderer_vertex_shaders.emplace_back(RendererInfo{ reflect::typehash<MeshRenderer>(), 
			std::move(
				Program{}
					.Attach(Shader{GL_VERTEX_SHADER, ""})
					.Link()
			)}
		);

		glGenVertexArrays(1, &vao_id);
	}

	void OpenGLState::SubmitBuffers(vector<RenderObject>&& mesh_render, vector<RenderObject>&& skinned_mesh_render)
	{
		object_buffer[curr_write_buffer].mesh_render         = std::move(mesh_render); 
		object_buffer[curr_write_buffer].skinned_mesh_render = std::move(skinned_mesh_render);
		SwapWritingBuffer();
	}

	void OpenGLState::RenderDrawBuffer()
	{
		// lock drawing buffer
		
		// render mesh renderers
		auto itr_to_mesh_vtx = std::find_if(renderer_vertex_shaders.begin(), renderer_vertex_shaders.end(),
			[](const auto& elem)->bool {
				return elem.typehash == reflect::typehash<MeshRenderer>();
			});

		pipeline.Use();
		pipeline.PushProgram(itr_to_mesh_vtx->vertex_shader);

		glBindVertexArray(vao_id);
		for (auto& elem : object_buffer[curr_draw_buffer].skinned_mesh_render)
		{
			// bind shader
			auto& material = static_cast<const OpenGLMaterial&>(*elem.material_instance.material);
			pipeline.PushProgram(material.GetShaderProgram());

			// bind attribs
			s_cast<OpenGLMesh&>(*elem.mesh).Bind(MeshRenderer::GetRequiredAttributes());

			// set uniforms

			// draw
			s_cast<OpenGLMesh&>(*elem.mesh).Draw();
		}
	}

	void OpenGLState::SwapWritingBuffer()
	{
		write_buffer_dirty = true;
	}
}
