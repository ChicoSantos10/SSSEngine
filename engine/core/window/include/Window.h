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

#include "WindowHandle.h"
#include "Platform.h"
#include "Attributes.h"

namespace SSSEngine::Core
{
    /**
     * @class Window
     * @brief A window to where we draw and receive input
     *
     */
    class Window final
    {
        public:
        Window(Platform::WindowVec position, Platform::WindowVec size, const Platform::WindowTitle &title,
               const Window *parent = nullptr);
        ~Window();
        Window(const Window &other) = delete;
        Window(Window &&other) = delete;
        Window &operator=(const Window &other) = delete;
        Window &operator=(Window &&other) = delete;

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE Platform::WindowHandle GetHandle() const noexcept
        {
            return m_handle;
        }

        // INVESTIGATE: Are these really noexcept
        // PERF: Is getting the rect first fine for performance or is the compiler not able to optimize? Does it even
        // matter
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE Platform::WindowVec GetSize() const noexcept
        {
            return Platform::GetWindowSize(m_handle);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE Platform::WindowRect GetRect() const noexcept
        {
            return Platform::GetWindowRect(m_handle);
        }

        void SetWindowTitle(Platform::WindowTitle title) const
        {
            Platform::SetWindowTitle(m_handle, title);
        }

        // LOW_PRIORITY: Potentially allow other types of fullscreen
        void SetBorderlessFullscreen(bool fullscreen) const
        {
            Platform::SetBorderlessFullscreen(m_handle, fullscreen);
        }

        void ToggleBorderlessFullscreen() const
        {
            Platform::ToggleBorderlessFullscreen(m_handle);
        }

        /* LOW_PRIORITY:
         *  - Add a method to change the window size
         *  - Add a method to change the window position
         *  - Add a method to set a new rect
         *  - Add a method to change the window icon
         *  - Add a method to change the window cursor
         *  - Add a method to minimize the window
         */

        private:
        Platform::WindowHandle m_handle;
    };
} // namespace SSSEngine::Core
