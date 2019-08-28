#pragma once
#include <idk.h>
#include <gfx/GraphicsSystem.h>
#include <win32/WindowsApplication.h>

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
		void RenderRenderBuffer() override;
	private:
		HDC   _windows_context;
		HGLRC _opengl_context;
		unique_ptr<OpenGLState> _opengl;
		color _clear_color;

		void CreateContext();
		void InitOpenGL();
		void InitResourceLoader();
		void DestroyContext();

		friend class OpenGLState;
	};
}