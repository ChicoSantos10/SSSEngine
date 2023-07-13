//
// Created by Francisco Santos on 13/07/2023.
//

#include <cassert>
#include <iostream>

#include "Win32Window.h"
#include "commctrl.h"
#include "atlBase.h"
#include "atlconv.h"

namespace Win32
{
	Win32Window::Win32Window(SSSEngine::Window::windowSize width, SSSEngine::Window::windowSize height,
	                         const std::string& title, WNDCLASSEX windowClass, HWND parent)
			: Window(width, height, title)
	{
		CA2W windowTitle(m_Title.c_str());

		m_Handle = CreateWindowEx(
				0,
				windowClass.lpszClassName,
				windowTitle,
				WS_OVERLAPPEDWINDOW,
				CW_USEDEFAULT, CW_USEDEFAULT,
				width, height,
				parent,
				nullptr,
				windowClass.hInstance,
				nullptr
		);

		assert(m_Handle && "Failed to create window.");

		if (SetWindowSubclass(m_Handle, &Win32Window::WindowProcedure, 0, reinterpret_cast<DWORD_PTR>(this)))
			std::cout << "Window subclassed." << std::endl;

		ShowWindow(m_Handle, SW_SHOW);
	}

	LRESULT Win32Window::WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass,
	                                     DWORD_PTR dwRefData)
	{
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

	void Win32Window::ChangeWindowTitle(std::string_view title)
	{
		SetWindowText(m_Handle, reinterpret_cast<LPCWSTR>(title.data()));
	}
} // Win32