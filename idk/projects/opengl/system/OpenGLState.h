#pragma once
#include <idk.h>
#include <gfx/RenderObject.h>
#include <opengl/program/Program.h>
#include <opengl/program/PipelineProgram.h>
#include <opengl/system/FrameBufferManager.h>

#include <opengl/PixelData.h>

namespace idk::ogl
{
	class Win32GraphicsSystem;
	class OpenGLState
	{
	public:
		void Setup();
		~OpenGLState();
		void GenResources();

		void RenderDrawBuffer();

		void ConvoluteCubeMap(const RscHandle<ogl::OpenGLCubemap>& handle);
		void ComputeBRDF(const RscHandle<ogl::Program>&);

		PixelData PickData(const vec2& pos);
		void IsPicking();

	private:

		Win32GraphicsSystem* sys{};
		// variables
		PipelineProgram          pipeline;
		FrameBufferManager       fb_man;
		GLuint                   vao_id = 0;
		GLuint                   particle_vao_id = 0;
		GLuint                   font_vao_id = 0;
		GLuint					 vbo_font_id = 0;

		GLuint object_vbo_id = 0;
		GLuint normal_vbo_id = 0;
		vector<mat4> object_transform;
		vector<mat4> normal_transform;

		RscHandle<OpenGLTexture> brdf_texture;
		//RscHandle<ShaderProgram> debug_fragment;
		bool is_picking = false;

		void FlushObjectTransforms();
	};
}