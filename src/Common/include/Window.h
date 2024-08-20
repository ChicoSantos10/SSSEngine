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

#pragma once

/* INVESTIGATE: This allows us to write platform specific which isn't allowed here
 *  However this allows us to have a clean window class with whatever handle the OS gives us
 *  The problem being to keep the renderer platform agnostic the create swap chain needs something that holds a HWND for
 *  DX12. For Vulkan this will not be the case
 * */
#include "Platform.h"

// TODO: Clean this up - Move to respective files
#include <concepts>

template<class T>
concept Numeric = std::integral<T> || std::floating_point<T>;

template<Numeric T>
struct Rect
{
	T x;
	T y;
	T width;
	T height;
};

namespace SSSEngine
{
	class Window
	{
	public:
		using WindowSize = int;
		using WindowRect = Rect<WindowSize>;

		Window(WindowSize x, WindowSize y, WindowSize width, WindowSize height, const WindowTitle& title,
		       Window* parent = nullptr);
		~Window() = default;

		// TODO: Create a macro for [[nodiscard]], const, const noexcept
		[[nodiscard]] inline WindowHandle GetHandle() const noexcept
		{
			return m_Handle;
		}

		[[nodiscard]] inline WindowSize GetWidth() const noexcept
		{
			return m_WindowPos.width;
		}

		[[nodiscard]] inline WindowSize GetHeight() const noexcept
		{
			return m_WindowPos.height;
		}

		[[nodiscard]] inline WindowRect GetRect() const noexcept
		{
			return m_WindowPos;
		}

		// INVESTIGATE: Is it necessary to ever get the title?
//		[[nodiscard]] inline std::string_view GetTitle() const
//		{ return m_Title; }

		void SetWindowTitle(const WindowTitle& title);

		// TODO: Potentially allow other types of fullscreen
		void SetBorderlessFullscreen(bool fullscreen);

		/* TODO:
		 *  - Add a method to change the window size
		 *  - Add a method to change the window position
		 *  - Add a method to set a new rect
		 *  - Add a method to change the window icon
		 *  - Add a method to change the window cursor
		 *  - Add a method to minimize the window
		 */
	private:
		// INVESTIGATE: Should we store the position? It would probably be better to just poll the window for it
		//  and just keep this as a way to potentially restore it. Ideally an alternative to storing would be nicer
		WindowRect m_WindowPos;
		WindowHandle m_Handle;
	};
} // SSSEngine

