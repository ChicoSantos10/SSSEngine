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

/*
 * @file
 * @brief Implementation of linux window functions
 *
 */

#include "Platform.h"
#include "WaylandWindow.h"
#include "WindowHandle.h"
#include "Bits.h"

#include <wayland-client-protocol.h>
#include <wayland-client.h>
#include <wayland-util.h>
#include <xdg-shell-client-protocol.h>

namespace SSSEngine::Platform
{

    // TODO: For now we will have support for a total of 5 windows. In the future we should allocate memory and
    // potentially increase the amount as the user creates and destroys said windows
    // We should also do something similar for Windows
    WaylandWindow Windows[5];
    int Index = 0;

    WindowId OpenWindow(WindowPos pos, WindowSize size, const WindowTitle &title, WindowId parent)
    {
        int windowId = Index++;
        WaylandWindow &window = Windows[windowId];
        window.surface = wl_compositor_create_surface(Compositor);
        window.shellSurface = xdg_wm_base_get_xdg_surface(XdgWmBase, window.surface);
        window.topLevel = xdg_surface_get_toplevel(window.shellSurface);
        window.position = pos;
        window.size = size;

        constexpr auto Configure = [](void *data, xdg_surface *shellSurface, u32 serial)
        {
            auto *window = static_cast<WaylandWindow *>(data);
            xdg_surface_ack_configure(shellSurface, serial);
            if(HasBitSet(window->state, WaylandWindowState::Resizing))
            {
                // TODO: Window resized: Recreate swap chain
                window->state = WithoutBits(window->state, WaylandWindowState::Resizing);
            }
        };
        window.surfaceListener = {.configure = Configure};

        xdg_surface_add_listener(window.shellSurface, &window.surfaceListener, &window);
        xdg_toplevel_set_title(window.topLevel, title);

        constexpr auto TopLevelConfigure = [](void *data, xdg_toplevel *toplevel, int w, int h, wl_array *states)
        {
            // INVESTIGATE: If width or height is 0 it means we must decide the window dimension. What should we do?
            if(w == 0 || h == 0)
            {
                return;
            }

            // INVESTIGATE: Should we overwrite the values here? Should we signal that the size changed?
            auto *window = static_cast<WaylandWindow *>(data);
            window->size.x = w;
            window->size.y = h;
            window->state = Join(window->state, WaylandWindowState::Resizing);
        };
        constexpr auto Close = [](void *data, xdg_toplevel *toplevel)
        {
            // TODO: Close the window
        };

        // TODO: Other events: ConfigureBounds and wmcapabilites
        window.toplevelListener = {.configure = TopLevelConfigure, .close = Close};
        xdg_toplevel_add_listener(window.topLevel, &window.toplevelListener, &window);

        xdg_surface_set_window_geometry(window.shellSurface, pos.x, pos.y, size.x, size.y);

        wl_surface_commit(window.surface);

        return {windowId};
    }

    WindowRect GetWindowRect(WindowId handle)
    {
        WaylandWindow window = Windows[handle];
        auto [x, y] = window.position;
        auto [w, h] = window.size;
        return {x, y, w, h};
    }

    WindowSize GetWindowSize(WindowId handle)
    {
        return Windows[handle].size;
    }

    void SetWindowTitle(WindowId handle, const WindowTitle &title)
    {
        WaylandWindow window = Windows[handle];
        xdg_toplevel_set_title(window.topLevel, title);
    }

    void SetBorderlessFullscreen(WindowId handle, bool fullscreen)
    {
        WaylandWindow window = Windows[handle];
        if(fullscreen)
        {
            xdg_toplevel_set_fullscreen(window.topLevel, nullptr);
        }
        else
        {
            xdg_toplevel_unset_fullscreen(window.topLevel);
        }
        wl_surface_commit(window.surface);
    }

    bool IsBorderlessFullscreen(WindowId handle)
    {
        return HasBitSet(Windows[handle].state, WaylandWindowState::FullScreen);
    }
} // namespace SSSEngine::Platform
