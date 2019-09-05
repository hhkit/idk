#include "pch.h"

#include <iostream>

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
	bool Windows::IsMouseDragging(Key key)
	{
		switch (key)
		{
		case Key::LButton:
			return leftHold;
			break;
		case Key::RButton:
			return rightHold;
			break;
		};

		return false;
	}
	char Windows::GetChar()
	{
		return _input_manager->GetChar();
	}
	LRESULT Windows::WndProc(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (_hWnd != hWnd)
			return DefWindowProc(_hWnd, message, wParam, lParam);

		for (auto& elem : winProcList)
			elem(_hWnd, message, wParam, lParam);

		ivec2 sSize = GetScreenSize();
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
		case WM_LBUTTONDOWN:
			old_screenpos = screenpos;
			screenpos.x = LOWORD(lParam);
			screenpos.y = HIWORD(lParam);

			leftHold = true;

			ndc_screendel = vec2{
			static_cast<float>(screenpos.x) / static_cast<float>(sSize.x),
			static_cast<float>(screenpos.y) / static_cast<float>(sSize.y) } -
			vec2{
			static_cast<float>(old_screenpos.x) / static_cast<float>(sSize.x),
			static_cast<float>(old_screenpos.y) / static_cast<float>(sSize.y) };

			screendel = screenpos - old_screenpos;

			_dragging = leftHold;

			std::cout << std::boolalpha << _dragging << std::endl;
			_input_manager->SetMouseDragging(_dragging);

			SetFocus(hWnd);
			SetActiveWindow(hWnd);
			_input_manager->SetMouseDown((int)wParam);

			break;
		case WM_RBUTTONDOWN:
			old_screenpos = screenpos;
			screenpos.x = LOWORD(lParam);
			screenpos.y = HIWORD(lParam);

			rightHold = true;

			ndc_screendel = vec2{
			static_cast<float>(screenpos.x) / static_cast<float>(sSize.x),
			static_cast<float>(screenpos.y) / static_cast<float>(sSize.y) } -
			vec2{
			static_cast<float>(old_screenpos.x) / static_cast<float>(sSize.x),
			static_cast<float>(old_screenpos.y) / static_cast<float>(sSize.y) };
			
			screendel = screenpos - old_screenpos;

			_dragging = rightHold;

			std::cout << std::boolalpha << _dragging << std::endl;
			_input_manager->SetMouseDragging(_dragging);

			SetFocus(hWnd);
			SetActiveWindow(hWnd);
			_input_manager->SetMouseDown((int)wParam);

			break;
		case WM_LBUTTONUP:
			old_screenpos = screenpos;
			screenpos.x = LOWORD(lParam);
			screenpos.y = HIWORD(lParam);

			leftHold = false;

			ndc_screendel = vec2{
			static_cast<float>(screenpos.x) / static_cast<float>(sSize.x),
			static_cast<float>(screenpos.y) / static_cast<float>(sSize.y) } -
			vec2{
			static_cast<float>(old_screenpos.x) / static_cast<float>(sSize.x),
			static_cast<float>(old_screenpos.y) / static_cast<float>(sSize.y) };

			screendel = screenpos - old_screenpos;

			_dragging = leftHold;

			std::cout << std::boolalpha << _dragging << std::endl;
			_input_manager->SetMouseDragging(_dragging);

			SetFocus(hWnd);
			SetActiveWindow(hWnd);
			_input_manager->SetMouseDown((int)wParam);

			break;
		case WM_RBUTTONUP:
			old_screenpos = screenpos;
			screenpos.x = LOWORD(lParam);
			screenpos.y = HIWORD(lParam);

			rightHold = false;

			ndc_screendel = vec2{
			static_cast<float>(screenpos.x) / static_cast<float>(sSize.x),
			static_cast<float>(screenpos.y) / static_cast<float>(sSize.y) } -
			vec2{
			static_cast<float>(old_screenpos.x) / static_cast<float>(sSize.x),
			static_cast<float>(old_screenpos.y) / static_cast<float>(sSize.y) };

			_dragging = rightHold;
			std::cout << std::boolalpha << _dragging << std::endl;
			_input_manager->SetMouseDragging(_dragging);

			screendel = screenpos - old_screenpos;
			//old_screenpos = screenpos;

			_input_manager->SetMouseUp((int)wParam);
			break;
		case WM_MOUSEMOVE:
			old_screenpos = screenpos;
			screenpos.x = LOWORD(lParam);
			screenpos.y = HIWORD(lParam);

			ndc_screendel = vec2{
			static_cast<float>(screenpos.x) / static_cast<float>(sSize.x),
			static_cast<float>(screenpos.y) / static_cast<float>(sSize.y) } -
			vec2{
			static_cast<float>(old_screenpos.x) / static_cast<float>(sSize.x),
			static_cast<float>(old_screenpos.y) / static_cast<float>(sSize.y) };

			screendel = screenpos - old_screenpos;
			//old_screenpos = screenpos;

			break;
		case WM_MOUSEWHEEL:
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
