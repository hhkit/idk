#include "pch.h"

#include <core/Core.h>
#include <glad/glad.h>
#include <idk_opengl/resource/OpenGLMeshFactory.h>
#include "OpenGLGraphicsSystem.h"

namespace idk::ogl
{
	void Win32GraphicsSystem::Init()
	{
		CreateContext();
		InitOpenGL();
		InitResourceLoader();
	}

	void Win32GraphicsSystem::Shutdown()
	{
		DestroyContext();
	}

	void Win32GraphicsSystem::BufferGraphicsState(
		span<MeshRenderer>    mesh_renderers, 
		span<const Transform> transforms, 
		span<const Parent>    parents)
	{

	}

	GraphicsAPI Win32GraphicsSystem::GetAPI()
	{
		return GraphicsAPI::OpenGL;
	}

	void Win32GraphicsSystem::RenderBuffer()
	{
		glViewport(0, 0, 800, 600);
		glClearColor(_clear_color.r, _clear_color.g, _clear_color.b, _clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT);

		::SwapBuffers(_windows_context);
	}
	void Win32GraphicsSystem::CreateContext()
	{
		_windows_context = GetDC(Core::GetSystem<Windows>().GetWindowHandle());

		DEVMODE devMode = { 0 };
		devMode.dmSize = sizeof(DEVMODE);
		BOOL b = EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &devMode);
		if (b == 0)
			throw;

		// create pixel format descriptor
		PIXELFORMATDESCRIPTOR pfdesc;
		memset(&pfdesc, 0, sizeof(PIXELFORMATDESCRIPTOR));

		pfdesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
		pfdesc.nVersion = 1;
		pfdesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER;
		pfdesc.iPixelType = PFD_TYPE_RGBA;
		pfdesc.cColorBits = (BYTE)devMode.dmBitsPerPel;//32; //24 bit color for front and back buffer
		pfdesc.cDepthBits = 24;//24 bit depth buffer - not used in this demo
		pfdesc.cStencilBits = 8; //8 bit stencil buffer - not used in this demo

		int pf = ChoosePixelFormat(_windows_context, &pfdesc);//checks if the graphics card can support the pixel format requested
		if (pf == 0)
		{
			ReleaseDC(Core::GetSystem<Windows>().GetWindowHandle(), _windows_context);
			throw;
		}


		BOOL ok = SetPixelFormat(_windows_context, pf, &pfdesc);
		if (!ok)
		{
			ReleaseDC(Core::GetSystem<Windows>().GetWindowHandle(), _windows_context);
			throw;
		}
		//set the OpenGL context
		_opengl_context = wglCreateContext(_windows_context);
		if (!_opengl_context)
		{
			ReleaseDC(Core::GetSystem<Windows>().GetWindowHandle(), _windows_context);
			throw;
		}


		ok = wglMakeCurrent(_windows_context, _opengl_context);
		if (!ok)
		{
			wglDeleteContext(_opengl_context);
			ReleaseDC(Core::GetSystem<Windows>().GetWindowHandle(), _windows_context);
			throw;
		}
	}
	void Win32GraphicsSystem::InitOpenGL()
	{
		gladLoadGL();
	}

	void Win32GraphicsSystem::InitResourceLoader()
	{
		Core::GetResourceManager().RegisterFactory<OpenGLMeshFactory>();
	}

	void Win32GraphicsSystem::DestroyContext()
	{
		if (_opengl_context)
		{
			if (!wglMakeCurrent(NULL, NULL))
			{
				//log
			}
		}

		if (!wglDeleteContext(_opengl_context))
		{
			//log
		}
		_opengl_context = NULL;

		if (_windows_context && !ReleaseDC(Core::GetSystem<Windows>().GetWindowHandle(), _windows_context))
		{
			_windows_context = NULL;
		}
	}
}
