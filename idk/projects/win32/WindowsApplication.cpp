#include "pch.h"

#include <iostream>
#include <direct.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#include <stdio.h>  
#include <locale>
#include <codecvt>
#include <commdlg.h>

#include <core/Core.h>

#include "InputManager.h"
#include "WindowsApplication.h"

namespace idk::win
{
	Windows::Windows(HINSTANCE _hInstance, int nCmdShow)
		: hInstance{ _hInstance }, _input_manager{std::make_unique<InputManager>()}
	{
		instance = this;
		// Initialize global strings
		//LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		//LoadStringW(hInstance, IDC_GAME, szWindowClass, MAX_LOADSTRING);

		//#ifdef _DEBUG
		{
			AllocConsole();
			AttachConsole(GetCurrentProcessId());
			FILE* pCout;
			freopen_s(&pCout, "conout$", "w", stdout); //returns 0
			freopen_s(&pCout, "conout$", "w", stderr); //returns 0
			SetConsoleTitle(L"IDK 0.1a");
		}
		//#endif

		MyRegisterClass();

		InitInstance(nCmdShow);
		//hAccelTable = LoadAccelerators(hInstance, 0);

	}

	Windows::~Windows() = default;

	void Windows::PollEvents()
	{
		MSG msg;
		_input_manager->SwapBuffers();
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

	}
	int Windows::GetReturnVal()
	{
		return retval;
	}
	ivec2 Windows::GetScreenSize() 
	{
		RECT rect;
		if (!GetClientRect(hWnd, &rect))
			return ivec2{};
		return ivec2((rect.right - rect.left), (rect.bottom - rect.top));
	}
	vec2 Windows::GetMouseScreenPos()
	{
		ivec2 sSize = GetScreenSize();
		return vec2{ 
			static_cast<float>(screenpos.x) / static_cast<float>(sSize.x),
			static_cast<float>(screenpos.y) / static_cast<float>(sSize.y) };
	}
	vec2 Windows::GetMouseScreenDel()
	{
		//ivec2 sSize = GetScreenSize();
		return ndc_screendel;
	}
	ivec2 Windows::GetMousePixelPos()
	{
		return screenpos;
	}
	ivec2 Windows::GetMousePixelDel()
	{
		return screendel;
	}
	ivec2 Windows::GetMouseScroll()
	{
		return _input_manager->GetMouseScroll();
	}
	void Windows::PushWinProcEvent(std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)> func)
	{
		winProcList.push_back(func); 
	}

	bool Windows::GetKeyDown(Key key)
	{
		return _input_manager->GetKeyDown(static_cast<int>(key));
	}
	bool Windows::GetKey(Key key)
	{
		return _input_manager->GetKey(static_cast<int>(key));
	}
	bool Windows::GetKeyUp(Key key)
	{
		return _input_manager->GetKeyUp(static_cast<int>(key));
	}
	char Windows::GetChar()
	{
		return _input_manager->GetChar();
	}

    bool Windows::SetFullscreen(bool fullscreen)
    {
        // stolen from chromium

        if (!_fullscreen)
        {
            // Save current window information.  We force the window into restored mode
            // before going fullscreen because Windows doesn't seem to hide the
            // taskbar if the window is in the maximized state.
            _saved_win_info.maximized = !!::IsZoomed(hWnd);
            if (_saved_win_info.maximized)
                ::SendMessage(hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
            _saved_win_info.style = GetWindowLong(hWnd, GWL_STYLE);
            _saved_win_info.ex_style = GetWindowLong(hWnd, GWL_EXSTYLE);
            GetWindowRect(hWnd, &_saved_win_info.window_rect);
        }

        _fullscreen = fullscreen;

        if (_fullscreen)
        {
            // Set new window style and size.
            SetWindowLong(hWnd, GWL_STYLE,
                          _saved_win_info.style & ~(WS_CAPTION | WS_THICKFRAME));
            SetWindowLong(hWnd, GWL_EXSTYLE,
                          _saved_win_info.ex_style & ~(WS_EX_DLGMODALFRAME |
                                                       WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

            // On expand, if we're given a window_rect, grow to it, otherwise do
            // not resize.
            MONITORINFO monitor_info;
            monitor_info.cbSize = sizeof(monitor_info);
            GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                            &monitor_info);
            const auto& rect = monitor_info.rcMonitor;
            SetWindowPos(hWnd, NULL, rect.left, rect.top,
                         rect.right - rect.left, rect.bottom - rect.top,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        }
        else
        {
            // Reset original window style and size.  The multiple window size/moves
            // here are ugly, but if SetWindowPos() doesn't redraw, the taskbar won't be
            // repainted.  Better-looking methods welcome.
            SetWindowLong(hWnd, GWL_STYLE, _saved_win_info.style);
            SetWindowLong(hWnd, GWL_EXSTYLE, _saved_win_info.ex_style);

                // On restore, resize to the previous saved rect size.
            const auto& rect = _saved_win_info.window_rect;
            SetWindowPos(hWnd, NULL, rect.left, rect.top,
                         rect.right - rect.left, rect.bottom - rect.top,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            if (_saved_win_info.maximized)
                ::SendMessage(hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
        }

        return true;
    }
	
	void Windows::SetTitle(string_view new_title)
	{
		std::wstring wide_str{ new_title.begin(), new_title.end() };
		SetWindowText(hWnd, wide_str.data());
	}

	string Windows::GetExecutableDir()
	{
		char buffer[MAX_PATH] = { 0 };

		// Get the program directory
		int bytes = GetModuleFileNameA(NULL, buffer, MAX_PATH);
		if (bytes == 0)
			std::cout << "[File System] Unable to get program directory." << std::endl;
		auto exe_dir = string{ buffer };
		auto pos = exe_dir.find_last_of("\\");
		return exe_dir.substr(0, pos);
	}
	string Windows::GetAppData()
	{
		char* env_buff;
		size_t len;
		auto err = _dupenv_s(&env_buff, &len, "appdata");
		if (err)
			std::cout << "[File System] Unable to get solution directory." << std::endl;
		auto appdata = string{ env_buff ? env_buff : "" };
		free(env_buff);
		return appdata;
	}
	string Windows::GetCurrentWorkingDir()
	{
		char buffer[MAX_PATH] = { 0 };
		if (!_getcwd(buffer, sizeof(buffer)))
			std::cout << "[File System] Unable to get solution directory." << std::endl;
		return string{ buffer };
	}
	opt<string> Windows::OpenFileDialog(const DialogOptions& dialog)
	{
		OPENFILENAMEA ofn;       // common dialog box structure
		CHAR szFile[260];       // buffer for file name
		HWND hwnd{};              // owner window

        string filter{ dialog.filter_name };
        filter += " (*";
        filter += dialog.extension;
        filter += ")";

        filter += '\0';
        filter += '*';
        filter += dialog.extension;
        filter += '\0';

		auto init = GetExecutableDir();

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = filter.c_str();
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = init.data();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Display the Open dialog box. 
		switch (dialog.type)
		{
		case DialogType::Save:
			if (GetSaveFileNameA(&ofn) == TRUE)
			{
                return ofn.lpstrFile;
			}
			break;
		case DialogType::Open:
			if (GetOpenFileNameA(&ofn) == TRUE)
			{
                return ofn.lpstrFile;
			}
		}
		return std::nullopt;
	}
	LRESULT Windows::WndProc(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (_hWnd != hWnd)
			return DefWindowProc(_hWnd, message, wParam, lParam);

		for (auto& elem : winProcList)
			elem(_hWnd, message, wParam, lParam);

		switch (message)
		{
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:

			if (_focused)
				_input_manager->SetKeyDown((int)wParam);
			break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
			_input_manager->SetKeyUp((int)wParam);
			break;
		case WM_CHAR:
			_input_manager->SetChar((char)wParam);
			break;
		case WM_MBUTTONDOWN:
			grabScreenCoordinates(lParam);

			if (_focused)
				_input_manager->SetMouseDown((int)Key::MButton);

			break;
		case WM_LBUTTONDOWN:
			grabScreenCoordinates(lParam);

			if (_focused)
				_input_manager->SetMouseDown((int)Key::LButton);

			break;
		case WM_RBUTTONDOWN:
			grabScreenCoordinates(lParam);

			if (_focused)
				_input_manager->SetMouseDown((int)Key::RButton);

			break;
		case WM_LBUTTONUP:
			grabScreenCoordinates(lParam);

			_input_manager->SetMouseDown((int)Key::LButton);

			break;
		case WM_RBUTTONUP:
			grabScreenCoordinates(lParam);

			_input_manager->SetMouseUp((int)Key::RButton);
			break;
		case WM_MBUTTONUP:
			grabScreenCoordinates(lParam);

			_input_manager->SetMouseUp((int)Key::MButton);
			break;
		case WM_MOUSEMOVE:
			grabScreenCoordinates(lParam);
			break;
		case WM_MOUSEWHEEL:
			_input_manager->SetMouseScroll(ivec2{ 0, GET_WHEEL_DELTA_WPARAM(wParam) });
			break;
		case WM_PAINT:
			ValidateRect(hWnd, 0);
			break;
		case WM_NCCREATE:
		{
			auto ptr = reinterpret_cast<CREATESTRUCTW*&>(lParam);
			OnScreenSizeChanged.Fire(ivec2{ ptr->cx, ptr->cy });
		}
		break;
		case WM_SETFOCUS:
			OnFocusGain.Fire();
			_focused = true;
			break;
		case WM_KILLFOCUS:
			OnFocusLost.Fire();
			_focused = false;
			_input_manager->FlushCurrentBuffer();
			break;
		case WM_SIZE:
			OnScreenSizeChanged.Fire(ivec2{ LOWORD(lParam), HIWORD(lParam) });
			break;
		case WM_DESTROY:
            OnClosed.Fire();
			PostQuitMessage(0);
			break;
		case WM_NCDESTROY:
			Core::Shutdown();
			break;
		case WM_QUIT:
			retval = (int)wParam;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}

	void Windows::grabScreenCoordinates(LPARAM lParam)
	{
		old_screenpos = screenpos;
		screenpos.x = LOWORD(lParam);
		screenpos.y = HIWORD(lParam);
		
		
		vec2 sSize = vec2{ GetScreenSize() };
		ndc_screendel = vec2{ screenpos.x, screenpos.y } / sSize - vec2{ old_screenpos.x, old_screenpos.y } / sSize;

		screendel = screenpos - old_screenpos;
	}

	HINSTANCE Windows::GetInstance()
	{
		return hInstance;
	}

	HWND Windows::GetWindowHandle()
	{
		return hWnd;
	}

	ATOM Windows::MyRegisterClass()
	{
		WNDCLASSEXW wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = ::WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = 0;  
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = 0;  
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = 0;

		return RegisterClassExW(&wcex);
	}

	BOOL Windows::InitInstance(int nCmdShow)
	{
        hWnd = CreateWindowW(szWindowClass, L"IDK 0.1a", WS_OVERLAPPEDWINDOW,
                             CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

		if (!hWnd)
		{
			return FALSE;
		}

		ShowWindow(hWnd, SW_MAXIMIZE);
		UpdateWindow(hWnd);

		return TRUE;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (idk::win::Windows::instance)  // See Note 1 below
		// Call member function if instance is available
		return idk::win::Windows::instance->WndProc(hWnd, message, wParam, lParam);
	else
		// Otherwise perform default message handling
		return DefWindowProc(hWnd, message, wParam, lParam);
}
