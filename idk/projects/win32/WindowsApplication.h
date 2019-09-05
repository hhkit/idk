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
		Windows(HINSTANCE hInstance, int nCmdShow);
		~Windows();
		void PollEvents() override;
		int GetReturnVal();
		void Init() override {}
		ivec2 GetScreenSize() override;
		vec2 GetMouseScreenPos() override; 
		vec2 GetMouseScreenDel() override;
		ivec2 GetMousePixelPos() override;
		ivec2 GetMousePixelDel() override;
		bool GetKeyDown(Key) override;
		bool GetKey(Key) override;
		bool GetKeyUp(Key) override;
		bool IsMouseDragging(Key) override;
		char GetChar() override;
		// windows
		bool SetFullscreen(bool) override { return false; }
		bool SetScreenSize(ivec2) override { return false; }
		void SwapBuffers() override {}
		void Shutdown() override {}
		void PushWinProcEvent(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> func);

		HINSTANCE GetInstance();
		HWND      GetWindowHandle();
	private:
		HINSTANCE hInstance;
		HWND      hWnd;
		HACCEL    hAccelTable;
		WCHAR     szWindowClass[MAX_LOADSTRING]{L"idk"};
		int       retval;
		ivec2	  screenpos;
		ivec2     old_screenpos;
		ivec2	  screendel;
		vec2	  ndc_screendel;

		bool	  rightHold{ false };
		bool      leftHold{ false };
		static inline Windows* instance = nullptr;

		unique_ptr<InputManager> _input_manager;

		ATOM MyRegisterClass();
		BOOL InitInstance(int nCmdShow);
		LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		friend LRESULT CALLBACK ::WndProc(HWND, UINT, WPARAM, LPARAM);

		vector<std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>> winProcList;

		bool _dragging{false};
	};
}

namespace idk
{
	using win::Windows;
}