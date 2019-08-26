#pragma once

#include "framework.h"

#include <Windows.h>
#include <idk.h>
#include <app/Application.h>



#define MAX_LOADSTRING 100

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

namespace idk
{
	class Windows 
		: public Application
	{
	public:
		Windows(HINSTANCE hInstance, int nCmdShow);
		void PollEvents() override;
		int GetReturnVal();
		void Init() override {}
		ivec2 GetScreenSize()const;
		vec2 GetMouseScreenPos() override { return vec2{}; }
		vec2 GetMouseScreenDel() override { return vec2{}; }
		bool GetKeyDown(CharKey) override { return false; }
		bool GetKey(CharKey) override { return false; }
		bool GetKeyUp(CharKey) override { return false; }
		bool GetKeyDown(SysKey) override { return false; }
		bool GetKey(SysKey) override { return false; }
		bool GetKeyUp(SysKey) override { return false; }
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
		static inline Windows* instance = nullptr;

		ATOM MyRegisterClass();
		BOOL InitInstance(int nCmdShow);
		LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		friend LRESULT CALLBACK ::WndProc(HWND, UINT, WPARAM, LPARAM);

		vector<std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>> winProcList;
	};
}