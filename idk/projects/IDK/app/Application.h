#pragma once
#include <idk.h>
#include <core/ISystem.h>

namespace idk
{
	class Application
		: public ISystem
	{
	public:
		// app
		virtual void PollEvents() = 0;

		// movement
		virtual vec2 GetMouseScreenPos() = 0;
		virtual vec2 GetMouseScreenDel() = 0;
		virtual bool GetKeyDown() = 0;
		virtual bool GetKey() = 0;
		virtual bool GetKeyUp() = 0;

		// windows
		virtual bool SetFullscreen(bool enable_fullscreen) = 0;
		virtual bool SetScreenSize(ivec2 target_size) = 0;
		virtual void SwapBuffers() = 0;
	};
}