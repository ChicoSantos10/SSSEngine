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
		Win32Window(windowSize width, windowSize height, const std::string& title, WNDCLASSEX windowClass, HWND parent = nullptr);

		[[nodiscard]] HWND GetHandle() const { return m_Handle; }
		void ChangeWindowTitle(std::string_view title) override;
	private:
		HWND m_Handle;

		static LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR idSubclass, DWORD_PTR dwRefData);
	};

} // Win32
