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

#include "Rect.h"
#include "StringView.h"
#include "Vector.h"
#include "Attributes.h"

namespace SSSEngine::Platform
{
    struct WindowId
    {
        int id{-1};

        operator int() const // NOLINT(*-explicit-constructor)
        {
            return id;
        }
    };

    using WindowSizeType = int;
    using WindowRect = Math::Rect<WindowSizeType, WindowSizeType>;
    using WindowPos = Math::Vector2<decltype(WindowRect::x)>;
    using WindowSize = Math::Vector2<decltype(WindowRect::width)>;

    /**
     * @brief Opens a platform window
     *
     * @param pos Where in the screen should it open
     * @param size The size of the window
     * @param title The title of the window
     * @param parent A window handle. Can be set to null if the window should not have a parent
     * @return A handle to the opened window
     */
    WindowId OpenWindow(WindowPos pos, WindowSize size, const Text::Utf8View &title, WindowId parent);

    /**
     * @brief Gets the window rect
     *
     * @param handle The window to get the rect from
     * @return The rect of the window
     */
    WindowRect GetWindowRect(WindowId handle);

    /**
     * @brief Gets the window size
     *
     * @param handle The window to get the size of
     * @return The Size of the window
     */
    WindowSize GetWindowSize(WindowId handle);

    /**
     * @brief Sets the window title
     *
     * @param handle The handle of the window to change the title
     * @param title The new title
     */
    void SetWindowTitle(WindowId handle, const Text::Utf8View &title);

    /**
     * @brief Either sets the window to a borderlessfullscreen mode or back to window mode
     *
     * @param handle The handle to the window to change
     * @param fullscreen True for fullscreen and false for windowed
     */
    void SetBorderlessFullscreen(WindowId handle, bool fullscreen);

    /**
     * @brief Is this window currently in borderless fullscreen mode
     *
     * @param handle The Id of the window to check
     * @return Whether the window is in borderless fullscreen
     */
    bool IsBorderlessFullscreen(WindowId handle);

    /**
     * @brief Goes fullscreen if windowed or windowed if fullscreen
     *
     * @param handle The handle to the window to chance
     */
    SSSENGINE_FORCE_INLINE
    void ToggleBorderlessFullscreen(WindowId handle)
    {
        SetBorderlessFullscreen(handle, !IsBorderlessFullscreen(handle));
    }
} // namespace SSSEngine::Platform
