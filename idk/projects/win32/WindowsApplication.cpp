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
	opt<string> Windows::OpenFileDialog(string_view extension)
	{
		OPENFILENAME ofn;       // common dialog box structure
		TCHAR szFile[260];       // buffer for file name
		HWND hwnd{};              // owner window

		std::wstring init = [&]()
		{ 
			auto exe_dir = GetExecutableDir();
			return std::wstring{ exe_dir.begin(), exe_dir.end() };
		}();

		// Initialize OPENFILENAME
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hwnd;
		ofn.lpstrFile = szFile;
		// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
		// use the contents of szFile to initialize itself.
		ofn.lpstrFile[0] = L'\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = L"Scene (.idscene)\0*.idscene\0All\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = init.data();
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		// Display the Open dialog box. 

		if (GetSaveFileName(&ofn) == TRUE)
		{
			auto filename = std::wstring{ ofn.lpstrFile };
			using convert_type = std::codecvt_utf8<wchar_t>;
			std::wstring_convert<convert_type, wchar_t> converter;

			return converter.to_bytes(filename);
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

			_input_manager->SetMouseDown((int)Key::MButton);

			break;
		case WM_LBUTTONDOWN:
			grabScreenCoordinates(lParam);

			_input_manager->SetMouseDown((int)Key::LButton);

			break;
		case WM_RBUTTONDOWN:
			grabScreenCoordinates(lParam);

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

		ShowWindow(hWnd, nCmdShow);
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
