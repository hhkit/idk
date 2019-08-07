#pragma once

#include "framework.h"
#include "resource.h"

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
		int GetReturnVal() override;
		void Init() override {}
		vec2 GetMouseScreenPos() override { return vec2{}; }
		vec2 GetMouseScreenDel() override { return vec2{}; }
		bool GetKeyDown(Key) override { return false; }
		bool GetKey(Key) override { return false; }
		bool GetKeyUp(Key) override { return false; }
		// windows
		bool SetFullscreen(bool) override { return false; }
		bool SetScreenSize(ivec2) override { return false; }
		void SwapBuffers() override {}
		void Shutdown() override {}

	private:
		HINSTANCE hInstance;
		HWND      hWnd;
		HACCEL    hAccelTable;
		WCHAR     szTitle[MAX_LOADSTRING];
		WCHAR     szWindowClass[MAX_LOADSTRING];
		int       retval;
		static inline Windows* instance = nullptr;

		ATOM MyRegisterClass();
		BOOL InitInstance(int nCmdShow);
		LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		friend LRESULT CALLBACK ::WndProc(HWND, UINT, WPARAM, LPARAM);
	};
}