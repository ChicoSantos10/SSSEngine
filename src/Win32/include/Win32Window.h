//
// Created by Francisco Santos on 13/07/2023.
//

#pragma once

#include <windows.h>

#include "Window.h"

namespace Win32
{
	class Win32Window : public SSSEngine::Window
	{
	public:
		Win32Window() = delete;
		Win32Window(WindowSize width, WindowSize height, const std::string& title, WNDCLASSEX windowClass, HWND parent = nullptr);

		[[nodiscard]] HWND GetHandle() const
		{ return m_Handle; }

		void ChangeWindowTitle(std::string title) override;
		void SetBorderlessFullscreen(bool fullscreen) override;

		static LRESULT CALLBACK MainWindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	private:
		struct OldValues
		{
			RECT Rect;
			int Style;
		};

		HWND m_Handle;
		OldValues prevValues{};

		static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass, DWORD_PTR dwRefData);
	};

} // Win32
