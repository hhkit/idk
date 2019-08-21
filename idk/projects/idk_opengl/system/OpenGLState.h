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
		void Setup();

		void SubmitBuffers(vector<RenderObject>&& mesh_render, vector<RenderObject>&& skinned_mesh_render);
		void RenderDrawBuffer();
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
		vector<RendererInfo>   renderer_vertex_shaders;
		GLuint                 vao_id = 0;

		// triple buffered render state
		array<RenderBuffer, 3> object_buffer;
		unsigned               curr_write_buffer  = 0;
		unsigned               curr_draw_buffer   = 1;
		bool                   write_buffer_dirty = false;
	};
}