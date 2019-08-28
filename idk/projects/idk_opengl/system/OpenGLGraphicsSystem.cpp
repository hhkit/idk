#include "pch.h"

#include <glad/glad.h>
#include <glad/glad_wgl.h>
#include <core/Core.h>
#include <gfx/MeshRenderer.h>
#include <res/ForwardingExtensionLoader.h>

#include <idk_opengl/resource/OpenGLMaterialFactory.h>
#include <idk_opengl/resource/OpenGLMeshFactory.h>
#include <idk_opengl/system/OpenGLState.h>

#include "OpenGLGraphicsSystem.h"

#include <iostream>

namespace idk::ogl
{
	Win32GraphicsSystem::Win32GraphicsSystem() = default;
	Win32GraphicsSystem::~Win32GraphicsSystem() = default;

	void Win32GraphicsSystem::Init()
	{
		CreateContext();
		InitOpenGL();
		InitResourceLoader();

		_opengl = std::make_unique<OpenGLState>();
		_opengl->Setup();
	}

	void Win32GraphicsSystem::LateInit()
	{
		_opengl->GenResources();
	}

	void Win32GraphicsSystem::Shutdown()
	{
		_opengl.reset();
		DestroyContext();
	}

	GraphicsAPI Win32GraphicsSystem::GetAPI()
	{
		return GraphicsAPI::OpenGL;
	}

	void Win32GraphicsSystem::RenderRenderBuffer()
	{
		glViewport(0, 0, 800, 600);
		glClearColor(0.f, 0.f, 0.25f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		_opengl->RenderDrawBuffer();

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
		int attribs[] =
		{
			WGL_CONTEXT_MAJOR_VERSION_ARB, 1,
			WGL_CONTEXT_MINOR_VERSION_ARB, 0,
			WGL_CONTEXT_FLAGS_ARB, 0,
			0
		};
		gladLoadWGL(_windows_context);
		auto new_context = wglCreateContextAttribsARB(_windows_context, _opengl_context, attribs);
		
		if (new_context)
		{
			wglMakeCurrent(NULL, NULL);
			wglDeleteContext(_opengl_context);
			_opengl_context = new_context;
			wglMakeCurrent(_windows_context, _opengl_context);
		}
		else
			throw;
		std::cout << (char*)glGetString(GL_VERSION);
	}

	void Win32GraphicsSystem::InitResourceLoader()
	{	
		Core::GetResourceManager().RegisterFactory<OpenGLMeshFactory>();
		Core::GetResourceManager().RegisterFactory<OpenGLMaterialFactory>();
		Core::GetResourceManager().RegisterExtensionLoader<ForwardingExtensionLoader<Material>>(".frag");
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
