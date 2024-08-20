//
// Created by Francisco Santos on 13/07/2023.
//

#include "Window.h"
#include "Win32Window.h"
#include "commctrl.h"

namespace SSSEngine
{
	// INVESTIGATE: Is this necessary?? It currently does not do anything useful
	LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
	                        DWORD_PTR dwRefData)
	{
		auto* window = reinterpret_cast<Window*>(dwRefData);

		switch (msg)
		{
			case WM_DESTROY:
			{
				RemoveWindowSubclass(hwnd, &WindowProcedure, 0);
				break;
			}
		}

		return DefSubclassProc(hwnd, msg, wParam, lParam);
	}


	Window::Window(Window::WindowSize x, Window::WindowSize y, Window::WindowSize width, Window::WindowSize height,
	               const WindowTitle& title, Window* parent)
			: m_WindowPos{x, y, width, height}
	{
		// TODO: Assert that class has been registered

		HMENU menu = CreateMenu();
		AppendMenu(menu, MF_STRING, 1, L"File");

		constexpr int childStyle = WS_CHILD | (WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU));
		int style = parent ? childStyle : WS_OVERLAPPEDWINDOW;

		m_Handle = CreateWindowEx(
				0,
				Win32::WindowClass.lpszClassName,
				title,
				style,
				CW_USEDEFAULT, CW_USEDEFAULT,
				width, height,
				parent ? (HWND) parent->m_Handle : nullptr,
				nullptr,
				Win32::WindowClass.hInstance,
				nullptr
		);

		assert(m_Handle && "Failed to create window.");
		assert(SetWindowSubclass((HWND) m_Handle, &WindowProcedure, 0,
		                         reinterpret_cast<DWORD_PTR>(this)) &&
		       "Window subclassed failed!");

		ShowWindow((HWND) m_Handle, SW_SHOW);
	}


	void Window::SetWindowTitle(const WindowTitle& title)
	{
		SetWindowText((HWND) m_Handle, title);
	}

	void Window::SetBorderlessFullscreen(bool fullscreen)
	{
		LONG styles = GetWindowLong((HWND) m_Handle, GWL_STYLE);

		//constexpr auto borderlessStyle = WS_OVERLAPPED;
		constexpr LONG borderlessStyle = WS_MAXIMIZE;

		bool isBorderless = styles & borderlessStyle;
		if (isBorderless && fullscreen)
			return;


		if (fullscreen)
		{
			RECT windowRect;
			GetWindowRect((HWND) m_Handle, &windowRect);

			m_WindowPos.x = windowRect.left;
			m_WindowPos.y = windowRect.top;
			m_WindowPos.width = windowRect.right - windowRect.left;
			m_WindowPos.height = windowRect.bottom - windowRect.top;

			const auto newStyle = styles | borderlessStyle;
			//prevValues.Style = GetWindowLongPtr(m_Handle, GWL_STYLE);
			SetWindowLongPtr((HWND) m_Handle, GWL_STYLE, newStyle);
			HMONITOR monitor = MonitorFromWindow((HWND) m_Handle, MONITOR_DEFAULTTONEAREST);

			MONITORINFOEX monitorInfo{};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			GetMonitorInfo(monitor, &monitorInfo);
			constexpr auto flags = SWP_FRAMECHANGED | SWP_NOACTIVATE;
			auto monitorRect = monitorInfo.rcMonitor;
			SetWindowPos((HWND) m_Handle,
			             HWND_TOP,
			             monitorRect.left,
			             monitorRect.top,
			             monitorRect.right - monitorRect.left,
			             monitorRect.bottom - monitorRect.top,
			             flags);

			ShowWindow((HWND) m_Handle, SW_MAXIMIZE);
		} else
		{
			// TODO: Create helper functions that can be inlined to help with flag operations to prevent mistakes
			const auto restoredStyle = styles & ~borderlessStyle;
			SetWindowLongPtr((HWND) m_Handle, GWL_STYLE, restoredStyle);

			SetWindowPos((HWND) m_Handle, HWND_TOP,
			             m_WindowPos.x,
			             m_WindowPos.y,
			             m_WindowPos.width,
			             m_WindowPos.height,
			             SWP_FRAMECHANGED);

			ShowWindow((HWND) m_Handle, SW_NORMAL);

			//ZeroMemory(&prevValues, sizeof(OldValues));
		}
	}
} // SSSEngine

