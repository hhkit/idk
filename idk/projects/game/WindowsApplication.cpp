#include "WindowsApplication.h"
#include <core/Core.h>

namespace idk
{
	Windows::Windows(HINSTANCE _hInstance, int nCmdShow)
		: hInstance{ _hInstance }
	{
		instance = this;
		// Initialize global strings
		LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadStringW(hInstance, IDC_GAME, szWindowClass, MAX_LOADSTRING);

		#ifdef _DEBUG
		{
			AllocConsole();
			AttachConsole(GetCurrentProcessId());
			FILE* pCout;
			freopen_s(&pCout, "conout$", "w", stdout); //returns 0
			freopen_s(&pCout, "conout$", "w", stderr); //returns 0
			SetConsoleTitle(szTitle);
		}
		#endif

		MyRegisterClass();

		InitInstance(nCmdShow);
		hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_GAME));

	}

	void Windows::PollEvents()
	{
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		retval = (int)msg.wParam;
	}
	int Windows::GetReturnVal()
	{
		return retval;
	}
	LRESULT Windows::WndProc(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (_hWnd != hWnd)
			return DefWindowProc(_hWnd, message, wParam, lParam);

		switch (message)
		{
		case WM_COMMAND:
		{
			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case IDM_EXIT:
				DestroyWindow(hWnd);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_PAINT:
			ValidateRect(hWnd, 0);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_NCDESTROY:
			Core::Shutdown();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
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
		wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAME));
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_GAME);
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

		return RegisterClassExW(&wcex);
	}

	BOOL Windows::InitInstance(int nCmdShow)
	{
		hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
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
	if (idk::Windows::instance)  // See Note 1 below
		// Call member function if instance is available
		return idk::Windows::instance->WndProc(hWnd, message, wParam, lParam);
	else
		// Otherwise perform default message handling
		return DefWindowProc(hWnd, message, wParam, lParam);
}
