#pragma once
#include <idk.h>
#include <gfx/RenderObject.h>
#include <idk_opengl/program/Program.h>
#include <idk_opengl/program/PipelineProgram.h>

namespace idk::ogl
{
	class OpenGLState
	{
	public:

		void SwapWritingBuffer();
		void SwapDrawingBuffer();

	private:
		// declarations
		struct RendererInfo
		{
			size_t  typehash {};
			Program vertex_shader;
		};

		struct RenderBuffer 
		{
			vector<RenderObject> mesh_render;
			vector<RenderObject> skinned_mesh_render;
		};

		// variables
		PipelineProgram        pipeline;
		vector<RendererInfo>   vertex;
		array<RenderBuffer, 3> object_buffer;
		unsigned               next_write_buffer;
		unsigned               next_draw_buffer;
	};
}