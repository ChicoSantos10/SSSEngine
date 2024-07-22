//
// Created by Francisco Santos on 13/07/2023.
//

#include "Win32Window.h"
#include "commctrl.h"
#include "atlBase.h"
#include "atlconv.h"

namespace Win32
{
	Win32Window::Win32Window(WindowSize width, WindowSize height,
	                         const std::string& title, WNDCLASSEX windowClass, HWND parent)
			: Window(width, height, title)
	{
		CA2W windowTitle(m_Title.c_str());

		HMENU menu = CreateMenu();
		AppendMenu(menu, MF_STRING, 1, L"File");

		constexpr int childStyle = WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
		int style = parent ? WS_CHILD | childStyle : WS_OVERLAPPEDWINDOW;

		m_Handle = CreateWindowEx(
				0,
				windowClass.lpszClassName,
				windowTitle,
				style,
				CW_USEDEFAULT, CW_USEDEFAULT,
				width, height,
				parent,
				nullptr,
				windowClass.hInstance,
				nullptr
		);

		assert(m_Handle && "Failed to create window.");
		assert(SetWindowSubclass(m_Handle, &Win32Window::WindowProcedure, 0, reinterpret_cast<DWORD_PTR>(this)) && "Window subclassed failed!");

		ShowWindow(m_Handle, SW_SHOW);
	}

	LRESULT Win32Window::WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
	                                     DWORD_PTR dwRefData)
	{
		auto* window = reinterpret_cast<Win32Window*>(dwRefData);

		switch (msg)
		{
			case WM_DESTROY:
			{
				RemoveWindowSubclass(hwnd, &Win32Window::WindowProcedure, 0);
				break;
			}
		}

		return DefSubclassProc(hwnd, msg, wParam, lParam);
	}

	LRESULT Win32Window::MainWindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
			case WM_SIZE:
			{
				RECT rect;
				GetClientRect(hwnd, &rect); // Client rect starts at [0, 0] so right and bottom are the width and height respectively

				// TODO: Window resize event: Needs to resize corresponding swap chain
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
			default:
				return DefWindowProcW(hwnd, msg, wParam, lParam);
		}
	}

	void Win32Window::ChangeWindowTitle(std::string title)
	{
		CA2W newTitle(title.data());
		SetWindowText(m_Handle, newTitle);
	}

	void Win32Window::SetBorderlessFullscreen(bool fullscreen)
	{
		auto styles = GetWindowLong(m_Handle, GWL_STYLE);

		constexpr auto borderlessStyle = WS_OVERLAPPED;

		bool isBorderless = (styles == borderlessStyle);
		if (isBorderless && fullscreen)
			return;

		if (fullscreen)
		{
			GetWindowRect(m_Handle, &prevValues.Rect);

			prevValues.Style = GetWindowLongPtr(m_Handle, GWL_STYLE);
			SetWindowLongPtr(m_Handle, GWL_STYLE, borderlessStyle);
			HMONITOR monitor = MonitorFromWindow(m_Handle, MONITOR_DEFAULTTONEAREST);

			MONITORINFOEX monitorInfo{};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			GetMonitorInfo(monitor, &monitorInfo);
			auto flags = SWP_FRAMECHANGED | SWP_NOACTIVATE;
			auto monitorRect = monitorInfo.rcMonitor;
			SetWindowPos(m_Handle,
						 HWND_TOP,
						 monitorRect.left,
						 monitorRect.top,
						 monitorRect.right - monitorRect.left,
			             monitorRect.bottom - monitorRect.top,
						 flags);

			ShowWindow(m_Handle, SW_MAXIMIZE);
		}
		else
		{
			SetWindowLongPtr(m_Handle, GWL_STYLE, prevValues.Style);

			RECT windowRect = prevValues.Rect;
			SetWindowPos(m_Handle, HWND_TOP,
						 windowRect.left,
						 windowRect.top,
						 windowRect.right - windowRect.left,
						 windowRect.bottom - windowRect.top,
						 SWP_FRAMECHANGED);

			ShowWindow(m_Handle, SW_NORMAL);

			//ZeroMemory(&prevValues, sizeof(OldValues));
		}
	}
} // Win32