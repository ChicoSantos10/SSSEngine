//
// Created by Francisco Santos on 13/07/2023.
//

#include "Window.h"
#include "Win32Window.h"
#include "commctrl.h"

namespace SSSEngine
{
	// INVESTIGATE: Is this necessary?? It currently does not do anything useful
	LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass, DWORD_PTR dwRefData)
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
	               const WindowTitle &title, Window *parent)
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

		SSSENGINE_ASSERT(m_handle && "Failed to create window.");
		SSSENGINE_ASSERT(
			SetWindowSubclass(static_cast<HWND>(m_handle), & WindowProcedure, 0, reinterpret_cast< DWORD_PTR>(this)) &&
			"Window subclassed failed!"
		);

		ShowWindow(static_cast<HWND>(m_handle), SW_SHOW);
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

	void Window::SetBorderlessFullscreen(const bool fullscreen)
	{
		const LONG styles = GetWindowLong(static_cast<HWND>(m_handle), GWL_STYLE);

		//constexpr auto borderlessStyle = WS_OVERLAPPED;
		constexpr LONG borderlessStyle = WS_MAXIMIZE;

		if (const bool isBorderless = styles & borderlessStyle; isBorderless && fullscreen)
			return;

		WINDOWPLACEMENT placement;
		placement.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(static_cast<HWND>(m_handle), &placement);

		// INVESTIGATE: Do we to resize the swap chain???
		if (fullscreen)
		{
			const auto newStyle = styles | borderlessStyle;
			//prevValues.Style = GetWindowLongPtr(m_Handle, GWL_STYLE);
			SetWindowLongPtr(static_cast<HWND>(m_handle), GWL_STYLE, newStyle);

			// INVESTIGATE: Should we store the monitor? Or should we just query it when we need it?
			HMONITOR monitor = MonitorFromWindow(static_cast<HWND>(m_handle), MONITOR_DEFAULTTONEAREST);

			MONITORINFOEX monitorInfo{};
			monitorInfo.cbSize = sizeof(MONITORINFOEX);
			GetMonitorInfo(monitor, &monitorInfo);
			constexpr auto flags = SWP_FRAMECHANGED | SWP_NOACTIVATE;
			const auto [left, top, right, bottom] = monitorInfo.rcMonitor;
			SetWindowPos(static_cast<HWND>(m_handle), HWND_TOP, left, top, right - left, bottom - top, flags);

			ShowWindow(static_cast<HWND>(m_handle), SW_MAXIMIZE);
		}
		else
		{
			// TODO: Create helper functions that can be inlined to help with flag operations to prevent mistakes
			const auto restoredStyle = styles & ~borderlessStyle;
			SetWindowLongPtr(static_cast<HWND>(m_handle), GWL_STYLE, restoredStyle);

			const auto [left, top, right, bottom] = placement.rcNormalPosition;
			SetWindowPos(static_cast<HWND>(m_handle),
			             HWND_TOP,
			             left,
			             top,
			             right - left,
			             bottom - top,
			             SWP_FRAMECHANGED
			);

			ShowWindow(static_cast<HWND>(m_handle), SW_NORMAL);

			//ZeroMemory(&prevValues, sizeof(OldValues));
		}
	}

	void Window::ToggleBorderlessFullscreen()
	{
		const LONG styles = GetWindowLong(static_cast<HWND>(m_handle), GWL_STYLE);

		//constexpr auto borderlessStyle = WS_OVERLAPPED;
		constexpr LONG borderlessStyle = WS_MAXIMIZE;

		const bool isBorderless = styles & borderlessStyle;
		SetBorderlessFullscreen(!isBorderless);
	}
} // SSSEngine

