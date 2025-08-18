/*  SSS Engine
    Copyright (C) 2025  Francisco Santos

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

/**
 * @file
 * @brief
 */

#pragma once

#include <windows.h>

// TODO: Remove iostream
#include <iostream>
#include "Bits.h"
#include "HelperMacros.h"

namespace SSSEngine::Platform::Win32
{
    SSSENGINE_GLOBAL WNDCLASSEXW WindowClass;

    // TODO: Move this to its own file responsible for win32 input
    // SSSENGINE_GLOBAL Core::Input::KeyboardCodes MapInput(LPARAM windows);

    SSSENGINE_GLOBAL LRESULT MainWindowProcedure(HWND hwnd, const UINT msg, const WPARAM wParam, const LPARAM lParam)
    {
        switch(msg)
        {
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            }
            case WM_CLOSE:
            {
                if(MessageBox(hwnd, L"Are you sure you want to quit?", L"SSSEngine", MB_YESNO) == IDYES)
                    DestroyWindow(hwnd);
                return 0;
            }
            case WM_SIZE:
            {
                return 0;
            }
            // TODO: Allow the option to use raw input
            //
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
                bool wasDown = SSSEngine::HasBitSet(lParam, 1 << 30);
                bool isDown = !SSSEngine::HasBitSet(lParam, 1 << 31);

                // NOTE: Prevents windows repeat messages
                if(wasDown == isDown)
                {
                    return 0;
                }
                // auto keyboard = Core::Input::KeyboardButtons;
                // using Code = Core::Input::KeyboardCodes;
                // using State = Core::Input::ButtonState;
                // State currentState = isDown ? State::Down : State::Released;
                // Code code = MapInput(lParam);
                // keyboard[code] = currentState;
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
                break;
            }
            default:
                break;
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }

    // Core::Input::KeyboardCodes MapInput(LPARAM windows)
    // {
    //     using Code = Core::Input::KeyboardCodes;
    //     switch(windows)
    //     {
    //         case VK_ESCAPE:
    //             return Code::Escape;
    //         case VK_SPACE:
    //             return Code::Space;
    //         case VK_UP:
    //             return Code::ArrowUp;
    //         case VK_DOWN:
    //             return Code::ArrowDown;
    //         case VK_LEFT:
    //             return Code::ArrowLeft;
    //         case VK_RIGHT:
    //             return Code::ArrowRight;
    //         default: // TODO: What should be the default? If we guarantee that it can never happen then we can do
    //                  // unreachable code and allow optimizations from the compiler
    //             return Code::Delete;
    //     }
    // }
} // namespace SSSEngine::Platform::Win32
