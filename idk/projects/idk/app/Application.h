#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <app/Keys.h>

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
		virtual bool GetKeyDown(Key) = 0;
		virtual bool GetKey(Key) = 0;
		virtual bool GetKeyUp(Key) = 0;
		virtual char GetChar() = 0;

		// windows
		virtual bool  SetFullscreen(bool enable_fullscreen) = 0;
		virtual bool  SetScreenSize(ivec2 target_size) = 0;
		virtual ivec2 GetScreenSize() { return ivec2{}; };
		virtual void  SwapBuffers() = 0;
	};
}