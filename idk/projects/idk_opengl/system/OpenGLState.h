#pragma once
#include <idk.h>
#include <gfx/RenderObject.h>
#include <idk_opengl/program/Program.h>
#include <idk_opengl/program/PipelineProgram.h>

namespace idk::ogl
{
	class Win32GraphicsSystem;
	class OpenGLState
	{
	public:
		void Setup();
		void GenResources();

		void RenderDrawBuffer();

	private:
		// declarations
		struct RendererInfo
		{
			size_t  typehash {};
			Program vertex_shader;
		};
		Win32GraphicsSystem* sys{};
		// variables
		PipelineProgram        pipeline;
		vector<RendererInfo>   renderer_vertex_shaders;
		GLuint                 vao_id = 0;
	};
}