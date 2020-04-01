#pragma once
#include <idk.h>
#include <core/ISystem.h>
#include <app/Keys.h>
#include <event/Signal.h>
#include <network/Device.h>

namespace idk
{
    enum class DialogType
    {
        Open, Save
    };
	struct DialogOptions
	{
        string_view filter_name;
        string_view extension;
        DialogType type = DialogType::Open;
	};

	class Application
		: public ISystem
	{
	public:
		Signal<ivec2> OnScreenSizeChanged;
		Signal<>      OnClosed;
		Signal<>      OnFocusLost;
		Signal<>      OnFocusGain;
	//	Signal<>      OnFullscreen;
	//	Signal<>      OnWindowed;
		// app
		virtual void PollEvents() = 0;
		virtual string Exec(string_view path, span<const char*> argv, bool wait) = 0;
		virtual void WaitForChildren() {};

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
		virtual bool  GetFullscreen()const { return false; };
		virtual bool  SetFullscreen(bool enable_fullscreen) = 0;
		virtual bool  SetScreenSize(ivec2 target_size) = 0;
		virtual ivec2 GetScreenSize() = 0 ;
		virtual void SetTitle([[maybe_unused]] string_view new_title) {};

		// file IO
		virtual string GetExecutableDir() = 0;
		virtual string GetAppData() = 0;
		virtual string GetCurrentWorkingDir() = 0;
		virtual opt<string> OpenFileDialog(const DialogOptions& dialog) = 0;

		// networking
		virtual vector<Device> GetNetworkDevices() = 0;
	};
}