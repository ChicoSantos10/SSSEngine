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

#include "Attributes.h"
#include "Platform.h"
#include "Rect.h"
#include "SwapChain.h"

namespace SSSEngine
{
    class Window final
    {
    public:
        using WindowSize = int;
        using WindowRect = SSSMath::Rect<WindowSize>;

        Window(WindowSize x, WindowSize y, WindowSize width, WindowSize height, const WindowTitle &title,
               const Window *parent = nullptr);
        ~Window();
        Window(const Window &other) = delete;
        Window(Window &&other) = delete;
        Window &operator=(const Window &other) = delete;
        Window &operator=(Window &&other) = delete;

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE WindowHandle GetHandle() const noexcept { return m_handle; }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE WindowSize GetWidth() const noexcept { return GetRect().width; }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE WindowSize GetHeight() const noexcept { return GetRect().height; }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE WindowRect GetRect() const noexcept;

        void SetWindowTitle(WindowTitle title) const;

        // LOW_PRIORITY: Potentially allow other types of fullscreen
        void SetBorderlessFullscreen(bool fullscreen) const;
        void ToggleBorderlessFullscreen() const;

        /* LOW_PRIORITY:
         *  - Add a method to change the window size
         *  - Add a method to change the window position
         *  - Add a method to set a new rect
         *  - Add a method to change the window icon
         *  - Add a method to change the window cursor
         *  - Add a method to minimize the window
         */

    private:
        WindowHandle m_handle;
        SSSRenderer::SwapChain m_swapChain{};
    };
} // namespace SSSEngine
