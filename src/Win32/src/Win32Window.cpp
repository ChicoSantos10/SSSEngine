//
// Created by Francisco Santos on 13/07/2023.
//

#include "Window.h"
#include "Win32Window.h"
#include "commctrl.h"

namespace SSSEngine
{
	// INVESTIGATE: Is this necessary?? It currently does not do anything useful
	//  A better alternative would be to have a method to update the window and poll there
	LRESULT WindowProcedure(HWND hwnd, const UINT msg, const WPARAM wParam, const LPARAM lParam, UINT_PTR idSubclass,
	                        DWORD_PTR dwRefData)
	{
		auto *window = reinterpret_cast<Window*>(dwRefData);

		switch (msg)
		{
		case WM_SYSCHAR: // Alt + Enter
			{
				window->ToggleBorderlessFullscreen();
				return 0;
			}
		case WM_DESTROY:
			{
				RemoveWindowSubclass(hwnd, &WindowProcedure, 0);
				break;
			}
		}

		return DefSubclassProc(hwnd, msg, wParam, lParam);
	}

	Window::Window(Window::WindowSize x, Window::WindowSize y, Window::WindowSize width, Window::WindowSize height,
	               const WindowTitle &title, const Window *parent)
	{
		// TODO: SSSENGINE_ASSERT that class has been registered

		HMENU menu = CreateMenu();
		AppendMenu(menu, MF_STRING, 1, L"File");

		constexpr int childStyle = WS_CHILD | (WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU));
		const int style = parent ? childStyle : WS_OVERLAPPEDWINDOW;

		m_handle = CreateWindowEx(0,
		                          SSSWin32::WindowClass.lpszClassName,
		                          title,
		                          style,
		                          CW_USEDEFAULT,
		                          CW_USEDEFAULT,
		                          width,
		                          height,
		                          parent ? static_cast<HWND>(parent->m_handle) : nullptr,
		                          nullptr,
		                          SSSWin32::WindowClass.hInstance,
		                          nullptr
		);

		// INVESTIGATE: This should probably not be asserts since we do want to check in runtime
		SSSENGINE_ASSERT(m_handle && "Failed to create window.");
		SSSENGINE_ASSERT(
			SetWindowSubclass(static_cast<HWND>(m_handle), & WindowProcedure, 0, reinterpret_cast< DWORD_PTR>(this)) &&
			"Window subclassed failed!"
		);

		ShowWindow(static_cast<HWND>(m_handle), SW_SHOWNORMAL);
	}

	Window::WindowRect Window::GetRect() const noexcept
	{
		// TODO: Handle GetWindowRect failure
		RECT rect;
		GetWindowRect(static_cast<HWND>(m_handle), &rect);

		return WindowRect{
			.x = static_cast<WindowSize>(rect.left),
			.y = static_cast<WindowSize>(rect.top),
			.width = static_cast<WindowSize>(rect.right - rect.left),
			.height = static_cast<WindowSize>(rect.bottom - rect.top)
		};
	}

	void Window::SetWindowTitle(const WindowTitle &title) const
	{
		SetWindowText(static_cast<HWND>(m_handle), title);
	}

	void Window::SetBorderlessFullscreen(const bool fullscreen) const
	{
		// TODO: This only allows one window to be fullscreen at a time.
		//		 What happens if we try to go fullscreen on a different one?
		// TODO: Remove static and replace with macro for functionLocal
		static WINDOWPLACEMENT window{.length = sizeof(window)};
		// IDEA: static Window* currentFullscreenWindow = nullptr
		// if(currentFullscreenWindow) currentFullscreenWindow->SetBorderlessFullscreen(false)

		const LONG styles = GetWindowLong(static_cast<HWND>(m_handle), GWL_STYLE);

		// INVESTIGATE: Do we to resize the swap chain???
		if (fullscreen)
		{
			// INVESTIGATE: Should we store the monitor? Or should we just query it when we need it?
			HMONITOR monitor = MonitorFromWindow(static_cast<HWND>(m_handle), MONITOR_DEFAULTTONEAREST);

			MONITORINFOEX monitorInfo{};
			monitorInfo.cbSize = sizeof(monitorInfo);
			if (GetMonitorInfo(monitor, &monitorInfo) && GetWindowPlacement(static_cast<HWND>(m_handle), &window))
			{
				SetWindowLongPtr(static_cast<HWND>(m_handle), GWL_STYLE, WithoutBits(styles, WS_OVERLAPPEDWINDOW));
				constexpr auto flags = SWP_FRAMECHANGED | SWP_NOOWNERZORDER;
				const auto [left, top, right, bottom] = monitorInfo.rcMonitor;
				SetWindowPos(static_cast<HWND>(m_handle), HWND_TOP, left, top, right - left, bottom - top, flags);

				//ShowWindow(static_cast<HWND>(m_handle), SW_SHOWMAXIMIZED);
			}
		}
		else
		{
			// TODO: Create helper functions that can be inlined to help with bitwise flag operations to prevent mistakes
			SetWindowLongPtr(static_cast<HWND>(m_handle), GWL_STYLE, styles | WS_OVERLAPPEDWINDOW);
			SetWindowPlacement(static_cast<HWND>(m_handle), &window);

			constexpr auto flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED;
			SetWindowPos(static_cast<HWND>(m_handle), nullptr, 0, 0, 0, 0, flags);

			//ShowWindow(static_cast<HWND>(m_handle), SW_SHOWNORMAL);
		}
	}

	void Window::ToggleBorderlessFullscreen() const
	{
		const LONG styles = GetWindowLong(static_cast<HWND>(m_handle), GWL_STYLE);

		const bool isBorderless = !HasBits(styles, WS_OVERLAPPEDWINDOW);
		SetBorderlessFullscreen(!isBorderless);
	}
} // SSSEngine
