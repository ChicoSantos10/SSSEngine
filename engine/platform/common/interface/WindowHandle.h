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

#include "Platform.h"
#include "Rect.h"
#include "Vector.h"

namespace SSSEngine::Platform
{
    using WindowSize = int;
    using WindowRect = Math::Rect<WindowSize>;
    using WindowVec = Math::Vector2<WindowSize>;

    /**
     * @brief Opens a platform window
     *
     * @param pos Where in the screen should it open
     * @param size The size of the window
     * @param title The title of the window
     * @param parent A window handle. Can be set to null if the window should not have a parent
     * @return A handle to the opened window
     */
    WindowHandle OpenWindow(WindowVec pos, WindowVec size, const WindowTitle &title, WindowHandle parent);

    /**
     * @brief Gets the window rect
     *
     * @param handle The window to get the rect from
     * @return The rect of the window
     */
    WindowRect GetWindowRect(WindowHandle handle);

    /**
     * @brief Gets the window size
     *
     * @param handle The window to get the size of
     * @return The Size of the window
     */
    WindowVec GetWindowSize(WindowHandle handle);

    /**
     * @brief Sets the window title
     *
     * @param handle The handle of the window to change the title
     * @param title The new title
     */
    void SetWindowTitle(WindowHandle handle, const WindowTitle &title);

    /**
     * @brief Either sets the window to a borderlessfullscreen mode or back to window mode
     *
     * @param handle The handle to the window to change
     * @param fullscreen True for fullscreen and false for windowed
     */
    void SetBorderlessFullscreen(WindowHandle handle, bool fullscreen);

    /**
     * @brief Goes fullscreen if windowed or windowed if fullscreen
     *
     * @param handle The handle to the window to chance
     */
    void ToggleBorderlessFullscreen(WindowHandle handle);
} // namespace SSSEngine::Platform
