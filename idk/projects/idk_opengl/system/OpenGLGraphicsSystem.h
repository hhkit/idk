#pragma once
#include <idk.h>
#include <gfx/GraphicsSystem.h>
#include <win32/WindowsApplication.h>
#include <event/Dispatcher.h>
#include <idk_opengl/resource/OpenGLCubemap.h>

namespace idk::ogl
{
	class OpenGLState;

	class Win32GraphicsSystem
		: public GraphicsSystem
	{
	public:
		Win32GraphicsSystem();
		~Win32GraphicsSystem();
		void Init()         override;
		void LateInit()     override;
		void Shutdown()     override;
		GraphicsAPI GetAPI() override;
		OpenGLState& Instance();
		void Prerender() override;
		void RenderRenderBuffer() override;
		void SwapBuffer() override;

		void EnqueueCubemapForConvolution(RscHandle<ogl::OpenGLCubemap>);
	private:
		HDC   _windows_context;
		HGLRC _opengl_context;
		unique_ptr<OpenGLState> _opengl;
		color _clear_color;
		ivec2 _viewport_size;
		RscHandle<ShaderProgram> prev_brdf;
		Dispatcher<void(Win32GraphicsSystem*, RscHandle<ogl::OpenGLCubemap>)> cubemaps_to_convolute;


		void CreateContext();
		void InitOpenGL();
		void InitResourceLoader();
		void ConvoluteCubeMap(RscHandle<ogl::OpenGLCubemap>);
		void DestroyContext();

		friend class OpenGLState;
	};
}