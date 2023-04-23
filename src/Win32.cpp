//
// Created by Francisco Santos on 22/04/2023.
//

#include <windows.h>
#include <stdio.h>
#include <iostream>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitConsole();
void CloseConsole();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	InitConsole();

	const WCHAR CLASS_NAME[] = L"SSS Engine";

	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	ATOM windowClass = RegisterClassExW(&wc);

	if (windowClass == 0)
	{
		OutputDebugStringW(L"Window class creation failed");
		return -1;
	}

	HWND hwnd = CreateWindowExW(
		0,
		CLASS_NAME,
		L"SSS Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	if (hwnd == NULL)
	{
		OutputDebugStringW(L"Window creation failed");
		return -1;
	}

	std::cout << "Hello World!" << std::endl;

	ShowWindow(hwnd, nShowCmd);

	MSG msg = { 0 };
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	CloseConsole();
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_MENUHILIGHT));

			EndPaint(hwnd, &ps);
			return 0;
		}
	}

	return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

void InitConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);
}

void CloseConsole()
{
	FreeConsole();
}