//
// Created by Francisco Santos on 13/07/2023.
//

#pragma once

#include <windows.h>

#include "Window.h"
#include "Renderer.h"

namespace Win32
{
	inline WNDCLASSEXW WindowClass;

	inline LRESULT MainWindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}
			case WM_CLOSE:
			{
				if (MessageBox(hwnd, L"Are you sure you want to quit?", L"SSSEngine", MB_YESNO) == IDYES)
					DestroyWindow(hwnd); // TODO: Window destroy event. Cleanup and removal of corresponding swap chain
				return 0;
			}
			case WM_ENTERSIZEMOVE:
			{
				// TODO: Pause Window (stop update and render)
			}
			case WM_EXITSIZEMOVE:
			{
				RECT rect;
				GetClientRect(hwnd,
				              &rect);

				// NOTE: Client rect starts at [0, 0] so right and bottom are the width and height respectively
				SSSEngineRenderer::ResizeSwapChain(rect.right, rect.bottom);
			}
			case WM_SIZE:
			{

				return 0;
			}
				// Mouse
			case WM_LBUTTONDOWN:
			{
				std::cout << "Left mouse button pressed" << std::endl;
				return 0;
			}
				// Keyboard
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			{
				switch (wParam)
				{
					case VK_ESCAPE:
					{
						PostQuitMessage(0);
						return 0;
					}
					case VK_SPACE:
					{
						std::cout << "Spacebar pressed" << std::endl;
						return 0;
					}
					case VK_UP:
					{
						std::cout << "Up arrow pressed" << std::endl;
						return 0;
					}
					default:
					{
						return DefWindowProcW(hwnd, msg, wParam, lParam);
					}
				}
			}
			case WM_SYSCHAR: // Alt + Enter
			{
				return 0;
			}
				// Cursor
			case WM_SETCURSOR:
			{
//				SetCursor(LoadCursor(nullptr, IDC_ARROW));
//				return true;
				return DefWindowProcW(hwnd, msg, wParam, lParam);
			}
			default: return DefWindowProcW(hwnd, msg, wParam, lParam);
		}

		// TODO: Set the default outside the switch to avoid duplication of the default return
	}
} // Win32
