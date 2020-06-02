#pragma once

#include "framework.h"

#include <Windows.h>
#include <idk.h>
#include <app/Application.h>



#define MAX_LOADSTRING 100

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

namespace idk::win
{
	class InputManager;

	class Windows 
		: public Application
	{
	public:
		Windows(HINSTANCE hInstance, int nCmdShow, HICON = 0);
		~Windows();
		void SetIcon(HICON icon);
		void PollEvents() override;
		string Exec(string_view path, span<const char*> argv, bool wait) override;
		void WaitForChildren() override;

		int GetReturnVal();
		void Init() override;
		void LateInit() override;
		ivec2 GetScreenSize() override;
		vec2 GetMouseScreenPos() override; 
		vec2 GetMouseScreenDel() override;
		ivec2 GetMousePixelPos() override;
		ivec2 GetMousePixelDel() override;
		ivec2 GetMouseScroll() override;

		bool GetKeyDown(Key) override;
		bool GetKey(Key) override;
		bool GetKeyUp(Key) override;
		char GetChar() override;
		void SwapInputBuffers() override;
		// windows
		bool GetFullscreen() const override;
		bool SetFullscreen(bool) override;
		bool SetScreenSize(ivec2) override { return false; }
		void SetTitle(string_view new_title) override;

		// network
		vector<Device> GetNetworkDevices() override;
		unique_ptr<Socket> CreateSocket() override;

		void Shutdown() override {}
		string GetExecutableDir() override;
		string GetAppData() override ;
		string GetCurrentWorkingDir() override;
		opt<string> OpenFileDialog(const DialogOptions& dialog) override;

		void PushWinProcEvent(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> func);

		HINSTANCE GetInstance();
		HWND      GetWindowHandle();
	private:
		HINSTANCE hInstance;
		HWND      hWnd;
		HACCEL    hAccelTable;
		HICON     icon{};
		WCHAR     szWindowClass[MAX_LOADSTRING]{L"idk"};
		int       retval;
		ivec2	  screenpos;
		ivec2     old_screenpos;
		ivec2	  screendel;
		vec2	  ndc_screendel;

        struct SavedWindowInfo
        {
            bool maximized;
            LONG style;
            LONG ex_style;
            RECT window_rect;    
        } _saved_win_info;

        bool _fullscreen = false;
		bool _dragging{ false };
		bool _focused{ true };
		vector<std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>> winProcList;
		unique_ptr<InputManager> _input_manager;

		vector<intptr_t> children;

		static inline Windows* instance = nullptr;

		ATOM MyRegisterClass();
		BOOL InitInstance(int nCmdShow);
		LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		void grabScreenCoordinates(LPARAM lParam);

		friend LRESULT CALLBACK ::WndProc(HWND, UINT, WPARAM, LPARAM);

	};
}

namespace idk
{
	using win::Windows;
}