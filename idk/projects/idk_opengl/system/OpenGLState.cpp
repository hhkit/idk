#include "pch.h"
#include <sstream>

#include <gfx/MeshRenderer.h>

#include <idk_opengl/resource/OpenGLMesh.h>
#include <idk_opengl/resource/OpenGLMaterial.h>

#include <core/Core.h>
#include <file/FileSystem.h>
#include <idk_opengl/system/OpenGLGraphicsSystem.h>
#include "OpenGLState.h"
#include <iostream>

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
		// generate mesh renderer
		auto load_mesh = Core::GetResourceManager().LoadFile("/assets/shader/mesh.vert");
		renderer_vertex_shaders.emplace_back(RendererInfo{ reflect::typehash<MeshRenderer>(), load_mesh.resources[0].As<ShaderProgram>() } );
	}


	void OpenGLState::RenderDrawBuffer()
	{
		auto& object_buffer = sys->object_buffer;
		auto& curr_write_buffer = sys->curr_write_buffer;
		auto& curr_draw_buffer = sys->curr_draw_buffer;
		curr_draw_buffer = curr_write_buffer;
		auto& curr_object_buffer = object_buffer[curr_draw_buffer];

		// range over cameras
		for(auto cam: curr_object_buffer.camera)
		{
			fb_man.SetRenderTarget({});

			//Bind frame buffers based on the camera's render target
			//Set the clear color according to the camera
			// lock drawing buffer
			
			// render mesh renderers
			auto itr_to_mesh_vtx = std::find_if(renderer_vertex_shaders.begin(), renderer_vertex_shaders.end(),
				[](const auto& elem)->bool {
					return elem.typehash == reflect::typehash<MeshRenderer>();
				});

			pipeline.Use();

			// per mesh render
			pipeline.PushProgram(itr_to_mesh_vtx->vertex_shader);
			pipeline.SetUniform("PerCamera.perspective_transform", cam.projection_matrix);

			glBindVertexArray(vao_id);
			for (auto& elem : curr_object_buffer.mesh_render)
			{
				// bind shader
				auto& material = elem.material_instance.material.as<OpenGLMaterial>();
				pipeline.PushProgram(material.GetShaderProgram());

				// bind attribs
				auto& mesh = elem.mesh.as<OpenGLMesh>();
				mesh.Bind(MeshRenderer::GetRequiredAttributes());

				// set uniforms
				// object uniforms

				auto obj_tfm = cam.view_matrix* elem.transform;
				pipeline.SetUniform("ObjectMat4s.object_transform", obj_tfm);
				pipeline.SetUniform("ObjectMat4s.normal_transform", obj_tfm.inverse().transpose());

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

		fb_man.ResetFramebuffer();
	}

}
