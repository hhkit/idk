#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <app/Keys.h>
#include <event/Signal.h>

namespace idk
{
	enum DialogOptions
	{
		Open,
		Save,
	};

	class Application
		: public ISystem
	{
	public:
		Signal<ivec2> OnScreenSizeChanged;
		Signal<>      OnClosed;
	//	Signal<>      OnMinimize;
	//	Signal<>      OnRestore;
	//	Signal<>      OnFullscreen;
	//	Signal<>      OnWindowed;
		// app
		virtual void PollEvents() = 0;

		// movement
		virtual vec2 GetMouseScreenPos() = 0;
		virtual vec2 GetMouseScreenDel() = 0;
		virtual ivec2 GetMousePixelPos() = 0;
		virtual ivec2 GetMousePixelDel() = 0;
		virtual ivec2 GetMouseScroll() = 0;
		virtual bool GetKeyDown(Key) = 0;
		virtual bool GetKey(Key) = 0;
		virtual bool GetKeyUp(Key) = 0;
		virtual char GetChar() = 0;

		// windows
		virtual bool  SetFullscreen(bool enable_fullscreen) = 0;
		virtual bool  SetScreenSize(ivec2 target_size) = 0;
		virtual ivec2 GetScreenSize() = 0 ;

		// file IO
		virtual string GetExecutableDir() = 0;
		virtual string GetAppData() = 0;
		virtual string GetCurrentWorkingDir() = 0;
		virtual opt<string> OpenFileDialog(string_view extension = ".*", DialogOptions save_dialog = DialogOptions::Open) = 0;
	};
}