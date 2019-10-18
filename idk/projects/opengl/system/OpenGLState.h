#pragma once
#include <idk.h>
#include <gfx/RenderObject.h>
#include <opengl/program/Program.h>
#include <opengl/program/PipelineProgram.h>
#include <opengl/system/FrameBufferManager.h>

namespace idk::ogl
{
	class Win32GraphicsSystem;
	class OpenGLState
	{
	public:
		void Setup();
		void GenResources();

		void RenderDrawBuffer();

		void ConvoluteCubeMap(const RscHandle<ogl::OpenGLCubemap>& handle);
		void ComputeBRDF(const RscHandle<ogl::Program>&);
	private:
		enum VertexShaders
		{
			Debug,
			NormalMesh,
			SkinnedMesh,
			SkyBox,
			Max
		};
		enum FragmentShaders
		{
			FDebug,
			FSkyBox,
			FShadow,
			FPicking,
			FMax
		};

		Win32GraphicsSystem* sys{};
		// variables
		PipelineProgram          pipeline;
		FrameBufferManager       fb_man;
		GLuint                   vao_id = 0;
		RscHandle<OpenGLTexture> brdf_texture;
		array<RscHandle<ShaderProgram>, VertexShaders::Max>   renderer_vertex_shaders;
		array<RscHandle<ShaderProgram>, FragmentShaders::FMax>   renderer_fragment_shaders;
		//RscHandle<ShaderProgram> debug_fragment;
	};
}