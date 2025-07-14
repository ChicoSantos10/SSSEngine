//
// Created by Francisco Santos on 13/07/2023.
//

#pragma once

#include <windows.h>

// TODO: Remove iostream
#include <iostream>
#include "HelperMacros.h"
#include "Renderer.h"
#include "Window.h"

namespace SSSWin32
{
    SSSENGINE_GLOBAL WNDCLASSEXW WindowClass;

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
                switch(wParam)
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
                        break;
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
                break;
            }
            default:
                break;
        }

        return DefWindowProcW(hwnd, msg, wParam, lParam);
    }
} // namespace SSSWin32
