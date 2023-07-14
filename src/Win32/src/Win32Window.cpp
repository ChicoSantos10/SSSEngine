//
// Created by Francisco Santos on 13/07/2023.
//

#include "Win32Window.h"
#include "commctrl.h"
#include "atlBase.h"
#include "atlconv.h"

namespace Win32
{
	Win32Window::Win32Window(SSSEngine::Window::windowSize width, SSSEngine::Window::windowSize height,
	                         const std::string& title, WNDCLASSEX windowClass, HWND parent)
		: SSSEngine::Window(width, height, title)
	{
		CA2W windowTitle(m_Title.c_str());

		HMENU menu = CreateMenu();
		AppendMenu(menu, MF_STRING, 1, L"File");

		int style = parent ? WS_CHILD | WS_BORDER | WS_CAPTION : WS_OVERLAPPEDWINDOW;
		std::cout << "Style: " << style << std::endl;

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

		if (!SetWindowSubclass(m_Handle, &Win32Window::WindowProcedure, 0, reinterpret_cast<DWORD_PTR>(this)))
			std::cout << "Window subclassed failed!" << std::endl;

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
				if(MessageBox(hwnd, L"Are you sure you want to quit?", L"SSSEngine", MB_YESNO) == IDYES)
					DestroyWindow(hwnd);
				return 0;
			}
			case WM_PAINT:
			{
				/*try
				{
					Renderer::DirectX::Render();
				}
				catch (const std::exception& e)
				{
					std::cout << e.what() << std::endl;
					system("pause");
				}*/
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);

				FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_MENUHILIGHT));

				EndPaint(hwnd, &ps);
				return 0;
			}
			/*case WM_SIZE:
				{
					RECT rect;
					GetClientRect(hwnd, &rect); // Client rect starts at [0, 0] so right and bottom are the width and height respectively

					Renderer::DirectX::Resize(rect.right, rect.bottom);
					return 0;
				}*/
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
				SetCursor(LoadCursor(nullptr, IDC_ARROW));
				return true;
			}
			default:
				return DefWindowProcW(hwnd, msg, wParam, lParam);
		}
	}

	void Win32Window::ChangeWindowTitle(std::string_view title)
	{
		CA2W newTitle(title.data());
		SetWindowText(m_Handle, newTitle);
	}
} // Win32