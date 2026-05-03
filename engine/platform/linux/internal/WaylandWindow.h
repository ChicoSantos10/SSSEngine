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
 * @brief Wayland specific data
 */

#pragma once

#include <wayland-client.h>
#include "HelperMacros.h"
#include "WindowHandle.h"
#include "xdg-shell-client-protocol.h"

namespace SSSEngine::Platform
{
    /**
     * @enum WaylandWindowState
     * @brief Represents the current state of the window
     *
     */
    enum class WaylandWindowState : u8
    {
        FullScreen = 0x01,
        Resizing = 0x02,
    };

    struct WaylandWindow
    {
        wl_surface *surface{};
        xdg_surface *shellSurface{};
        xdg_toplevel *topLevel{};
        xdg_surface_listener surfaceListener;
        xdg_toplevel_listener toplevelListener;
        WindowPos position; // INVESTIGATE: Should we keep this? I don't think we should care in wayland since there
                            // isnt a way to poll it and no way to know when the window moves
        WindowSize size;
        WaylandWindowState state{};
    };

    SSSENGINE_GLOBAL
    wl_display *Display;
    SSSENGINE_GLOBAL
    wl_registry *Registry;
    SSSENGINE_GLOBAL
    wl_compositor *Compositor;
    SSSENGINE_GLOBAL
    wl_shell *Shell;
    SSSENGINE_GLOBAL
    xdg_wm_base *XdgWmBase;
} // namespace SSSEngine::Platform
