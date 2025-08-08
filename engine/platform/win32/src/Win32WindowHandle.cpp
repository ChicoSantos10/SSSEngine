/*  SSS Engine
    Copyright (C) 2024  Francisco Santos

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "Platform.h"
#include "Win32Window.h"
#include "WindowHandle.h"
#include "Bits.h"
#include "Debug.h"
#include "commctrl.h"

namespace SSSEngine::Platform
{
    namespace Win32
    {
        // INVESTIGATE: Is this necessary?? It currently does not do anything useful
        //  A better alternative would be to have a method to update the window and poll there
        LRESULT
        WindowProcedure(HWND hwnd, const UINT msg, const WPARAM wParam, const LPARAM lParam, UINT_PTR idSubclass,
                        const DWORD_PTR dwRefData)
        {
            const auto *window = reinterpret_cast<WindowHandle *>(dwRefData); // NOLINT(*-no-int-to-ptr)

            switch(msg)
            {
                case WM_ENTERSIZEMOVE:
                {
                    // TODO: Pause Window (stop update and render)
                    break;
                }
                case WM_EXITSIZEMOVE:
                {
                    RECT rect;
                    GetClientRect(hwnd, &rect);

                    // NOTE: Client rect starts at [0, 0] so right and bottom are the width and height respectively
                    // TODO: Resize the swap chain on window resize. Create an event?
                    // Renderer::ResizeSwapChain(*window);
                    break;
                }
                case WM_SYSCHAR: // Alt + Enter
                {
                    // window->ToggleBorderlessFullscreen();
                    return 0;
                }
                case WM_DESTROY:
                {
                    RemoveWindowSubclass(hwnd, &WindowProcedure, 0);
                    break;
                }
                default:;
            }

            return DefSubclassProc(hwnd, msg, wParam, lParam);
        }
    } // namespace Win32

    WindowHandle OpenWindow(const WindowVec pos, const WindowVec size, const WindowTitle &title, WindowHandle parent)
    {
        auto [x, y] = pos;
        auto [width, height] = size;

        // TODO: assert max values as well
        // INVESTIGATE: Run time check?
        SSSENGINE_ASSERT((x >= 0 || x == CW_USEDEFAULT) && (y >= 0 || y == CW_USEDEFAULT) && width > 0 && height > 0 &&
                         "Window size is invalid");

        constexpr int ChildStyle = WS_CHILD | (WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU));
        const int style = parent ? ChildStyle : WS_OVERLAPPEDWINDOW;

        using namespace Win32;
        HWND handle = CreateWindowEx(0,
                                     WindowClass.lpszClassName,
                                     title,
                                     style,
                                     x,
                                     y,
                                     width,
                                     height,
                                     static_cast<HWND>(parent),
                                     nullptr,
                                     WindowClass.hInstance,
                                     nullptr);

        // INVESTIGATE: What can we do with this?
        const auto menu = CreateMenu();
        AppendMenu(menu, MF_STRING, 1, L"File");

        // INVESTIGATE: This should probably not be asserts since we do want to check in runtime
        SSSENGINE_ASSERT(handle && "Failed to create window.");
        SSSENGINE_ASSERT(SetWindowSubclass(
                             static_cast<HWND>(handle), &Win32::WindowProcedure, 0, reinterpret_cast<DWORD_PTR>(handle)) &&
                         "Window subclassed failed!");

        ShowWindow(static_cast<HWND>(handle), SW_SHOWNORMAL);

        return handle;
    }

    WindowRect GetWindowRect(WindowHandle handle)
    {
        // TODO: Handle GetWindowRect failure
        RECT rect;
        GetWindowRect(static_cast<HWND>(handle), &rect);

        // INVESTIGATE: Use width and height on rect vs top left, bottom right
        return WindowRect{.x = static_cast<WindowSize>(rect.left),
                          .y = static_cast<WindowSize>(rect.top),
                          .width = static_cast<WindowSize>(rect.right - rect.left),
                          .height = static_cast<WindowSize>(rect.bottom - rect.top)};
    }

    WindowVec GetWindowSize(WindowHandle handle)
    {
        RECT localRect;
        GetClientRect(static_cast<HWND>(handle), &localRect);

        return {localRect.right, localRect.bottom};
    }

    void SetWindowTitle(WindowHandle handle, const WindowTitle &title)
    {
        SetWindowText(static_cast<HWND>(handle), title);
    }

    void SetBorderlessFullscreen(WindowHandle handle, bool fullscreen, LONG styles)
    {
        // TODO: This only allows one window to be fullscreen at a time.
        //		 What happens if we try to go fullscreen on a different one?
        SSSENGINE_FUNCTION_LOCAL WINDOWPLACEMENT window{.length = sizeof(window)};
        // IDEA: static Window* currentFullscreenWindow = nullptr
        // if(currentFullscreenWindow) currentFullscreenWindow->SetBorderlessFullscreen(false)

        // INVESTIGATE: Do we need to resize the swap chain???
        if(fullscreen)
        {
            // INVESTIGATE: Should we store the monitor? Or should we just query it when we need it?
            const auto monitor = MonitorFromWindow(static_cast<HWND>(handle), MONITOR_DEFAULTTONEAREST);

            MONITORINFOEX monitorInfo{};
            monitorInfo.cbSize = sizeof(monitorInfo);
            if(GetMonitorInfo(monitor, &monitorInfo) && GetWindowPlacement(static_cast<HWND>(handle), &window))
            {
                SetWindowLongPtr(static_cast<HWND>(handle), GWL_STYLE, WithoutBits(styles, WS_OVERLAPPEDWINDOW));
                constexpr auto Flags = SWP_FRAMECHANGED | SWP_NOOWNERZORDER;
                const auto [left, top, right, bottom] = monitorInfo.rcMonitor;
                SetWindowPos(static_cast<HWND>(handle), HWND_TOP, left, top, right - left, bottom - top, Flags);

                // ShowWindow(static_cast<HWND>(m_handle), SW_SHOWMAXIMIZED);
            }
        }
        else
        {
            SetWindowLongPtr(static_cast<HWND>(handle), GWL_STYLE, styles | WS_OVERLAPPEDWINDOW);
            SetWindowPlacement(static_cast<HWND>(handle), &window);

            constexpr auto Flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED;
            SetWindowPos(static_cast<HWND>(handle), nullptr, 0, 0, 0, 0, Flags);

            // ShowWindow(static_cast<HWND>(m_handle), SW_SHOWNORMAL);
        }
    }

    void SetBorderlessFullscreen(WindowHandle handle, bool fullscreen)
    {
        const LONG styles = GetWindowLong(static_cast<HWND>(handle), GWL_STYLE);

        SetBorderlessFullscreen(handle, fullscreen, styles);
    }

    void ToggleBorderlessFullscreen(WindowHandle handle)
    {
        const LONG styles = GetWindowLong(static_cast<HWND>(handle), GWL_STYLE);

        const bool isBorderless = !HasBitSet(styles, WS_OVERLAPPEDWINDOW);
        SetBorderlessFullscreen(handle, !isBorderless);
    }
} // namespace SSSEngine::Platform
