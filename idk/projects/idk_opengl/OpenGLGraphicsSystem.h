#pragma once
#include <idk.h>
#include <gfx/GraphicsSystem.h>
#include <win32/WindowsApplication.h>

namespace idk::ogl
{
	class Win32GraphicsSystem
		: public GraphicsSystem
	{
	public:
		void Init()         override;
		void Shutdown()     override;
		void RenderBuffer() override;
	private:
		HDC   _windows_context;
		HGLRC _opengl_context;
		color _clear_color;
	};
}