//
// Created by Francisco Santos on 22/04/2023.
//

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <windowsx.h>
#include <xinput.h>
#include <math.h>

LRESULT CALLBACK WindowMessageCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InitConsole();
void CloseConsole();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	InitConsole();

	const WCHAR CLASS_NAME[] = L"SSS Engine";

	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowMessageCallback;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	if (RegisterClassExW(&wc) == 0)
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
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (hwnd == nullptr)
	{
		OutputDebugStringW(L"Window creation failed");
		return -1;
	}

	std::cout << "Hello World!" << std::endl;

	ShowWindow(hwnd, nShowCmd);
	bool isRunning = true;
	while (isRunning)
	{
		MSG msg = { 0 };
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		// Gamepad
		DWORD dwResult;
		for (int i = 0; i < XUSER_MAX_COUNT; ++i)
		{
			XINPUT_STATE state;
			ZeroMemory(&state, sizeof(XINPUT_STATE));

			dwResult = XInputGetState(i, &state);
			if (dwResult == ERROR_SUCCESS)
			{
				// Controller is connected
				XINPUT_GAMEPAD gamepad = state.Gamepad;

				if (gamepad.wButtons & XINPUT_GAMEPAD_A)
				{
					// Vibrations
					XINPUT_VIBRATION vibration;
					ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
					vibration.wLeftMotorSpeed = 1000;
					vibration.wRightMotorSpeed = 2000;
					XInputSetState(i, &vibration);
				}
				else
				{
					// Stop vibrations
					XINPUT_VIBRATION vibration;
					ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
					XInputSetState(i, &vibration);
				}

				float leftThumbX = gamepad.sThumbLX;
				float leftThumbY = gamepad.sThumbLY;

				float magnitude = sqrt(leftThumbX * leftThumbX + leftThumbY * leftThumbY);
				float normalizedLX = leftThumbX / magnitude;
				float normalizedLY = leftThumbY / magnitude;

				float normalizedMagnitude = 0;
				if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
				{
					//clip the magnitude at its expected maximum value
					if (magnitude > 32767)
						magnitude = 32767;

					magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

					normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
				}
				else
				{
					magnitude = 0.0;
					normalizedMagnitude = 0.0;
				}
			}
			else
			{
				// Controller is not connected
			}
		}
	}

	CloseConsole();
	return 0;
}

LRESULT CALLBACK WindowMessageCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
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
		case WM_LBUTTONDOWN:
		{
			std::cout << "Left mouse button pressed" << std::endl;
			return 0;
		}
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
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