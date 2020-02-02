#include "pch.h"

#include <thread>
#include <iostream>
#include <direct.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#include <stdio.h>  
#include <locale>
#include <codecvt>
#include <commdlg.h>

#include <core/Core.h>
#include <process.h>

#include "InputManager.h"
#include "WindowsApplication.h"

#include <ds/span.inl>
#include <event/Signal.inl>

namespace idk::win
{
	Windows::Windows(HINSTANCE _hInstance, int nCmdShow)
		: hInstance{ _hInstance }, _input_manager{std::make_unique<InputManager>()}
	{
		instance = this;
		// Initialize global strings
		//LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		//LoadStringW(hInstance, IDC_GAME, szWindowClass, MAX_LOADSTRING);

		#ifdef _DEBUG
		{
			AllocConsole();
			AttachConsole(GetCurrentProcessId());
			FILE* pCout;
			freopen_s(&pCout, "conout$", "w", stdout); //returns 0
			freopen_s(&pCout, "conout$", "w", stderr); //returns 0
			SetConsoleTitle(L"IDK 0.1a");
		}
		#endif

		MyRegisterClass();

		InitInstance(nCmdShow); 
		SetFullscreen(true);
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
//#pragma optimize ("", off)

	void Windows::Exec(string_view path, span<const char*> argv, bool wait)
	{
		vector<const char*> args{ path.data() };
		for (auto& elem : argv)
			args.emplace_back(elem);
		args.emplace_back(nullptr);

		auto ret = _spawnvp(wait ? P_WAIT : P_NOWAIT, path.data(), args.data());
		if (wait)
			LOG_TO(LogPool::SYS, "Executed %s with retcode %d", path.data(), ret);
		else
			LOG_TO(LogPool::SYS, "Executing %s with child %p", path.data(), ret);
		if (!wait)
		{
			if (children.size() >= std::thread::hardware_concurrency())
				WaitForChildren();
			children.emplace_back(ret);
		}
	}
	void Windows::WaitForChildren()
	{
		for (auto& elem : children)
		{
			int ret;
			_cwait(&ret, elem, 0);
			LOG_TO(LogPool::SYS, "Child %p ended with retcode %d", elem, ret);
		}
		children.clear();
	}
//#pragma optimize ("", on)
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

	string adapter_to_short_name(PIP_ADAPTER_ADDRESSES aa)
	{
		char buf[BUFSIZ];
		memset(buf, 0, BUFSIZ);
		WideCharToMultiByte(CP_ACP, 0, aa->FriendlyName, wcslen(aa->FriendlyName), buf, BUFSIZ, NULL, NULL);
		return buf;
	}

	string adapter_to_device_name(PIP_ADAPTER_ADDRESSES aa)
	{
		char buf[BUFSIZ];
		memset(buf, 0, BUFSIZ);
		WideCharToMultiByte(CP_ACP, 0, aa->Description, wcslen(aa->Description), buf, BUFSIZ, NULL, NULL);
		return buf;
	}

	std::optional<Address> adapter_to_address(PIP_ADAPTER_UNICAST_ADDRESS ua)
	{
		char buf[BUFSIZ];

		int family = ua->Address.lpSockaddr->sa_family;
		memset(buf, 0, BUFSIZ);
		getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
		if (family == AF_INET) // IPV4
		{
			in_addr addr;
			if (InetPtonA(family, buf, &addr) != 1)
				return {};
			return Address{ addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2,addr.S_un.S_un_b.s_b3 ,addr.S_un.S_un_b.s_b4 };
		}
		if (family == AF_INET6) // IPV6
		{
			in6_addr addr;
			if (InetPtonA(family, buf, &addr) != 1)
				return {};
			
		}
		return {};
	}

	vector<Device> Windows::GetNetworkDevices()
	{
		DWORD rv, size;
		PIP_ADAPTER_ADDRESSES adapter_addresses;
		PIP_ADAPTER_UNICAST_ADDRESS ua;

		rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
		if (rv != ERROR_BUFFER_OVERFLOW) 
			return {};

		adapter_addresses = (PIP_ADAPTER_ADDRESSES)malloc(size);

		rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapter_addresses, &size);
		if (rv != ERROR_SUCCESS) 
		{
			free(adapter_addresses);
			return {};
		}
		vector<Device> devices;
		for (auto aa = adapter_addresses; aa != NULL; aa = aa->Next) {
			Device d;
			d.name = adapter_to_short_name(aa);
			d.fullname = adapter_to_device_name(aa);
			for (ua = aa->FirstUnicastAddress; ua != NULL; ua = ua->Next) {
				auto get_addr = adapter_to_address(ua);
				if (get_addr)
					d.ip_addresses.emplace_back(*get_addr);
			}
			devices.emplace_back(d);
		}

		free(adapter_addresses);

		return devices;
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

	BOOL Windows::InitInstance([[maybe_unused]]int nCmdShow)
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
