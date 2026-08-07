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
 * @brief Linux Entry point
 */

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <wayland-client.h>

#include "AsciiEncoding.h"
#include "CString.h"
#include "Formatter.h"
#include "Platform.h"
#include "Types.h"
#include "WaylandWindow.h"
#include "Logger.h"
#include "xdg-shell-client-protocol.h"

int main(int argc, char *argv[])
{
    using namespace SSSEngine::Platform;

    Display = wl_display_connect(nullptr);
    if(!Display)
    {
        SSSENGINE_LOG_ERROR("Failed to connect to wayland display");
        return -1;
    }

    Registry = wl_display_get_registry(Display);
    auto registryHandler = [](void *data, wl_registry *registry, u32 id, const char *interface, u32 version)
    {
        using namespace SSSEngine::Text;

        if(StringEqual<AsciiEncoding>(interface, wl_compositor_interface.name))
        {
            Compositor = static_cast<wl_compositor *>(wl_registry_bind(registry, id, &wl_compositor_interface, version));
        }
        else if(StringEqual<AsciiEncoding>(interface, wl_shell_interface.name))
        {
            Shell = static_cast<wl_shell *>(wl_registry_bind(registry, id, &wl_shell_interface, version));
        }
        else if(StringEqual<AsciiEncoding>(interface, xdg_wm_base_interface.name))
        {
            XdgWmBase = static_cast<xdg_wm_base *>(wl_registry_bind(registry, id, &xdg_wm_base_interface, version));
            const xdg_wm_base_listener listener = {.ping = [](void *data, xdg_wm_base *xdg, u32 serial)
                                                   { xdg_wm_base_pong(xdg, serial); }};
            xdg_wm_base_add_listener(XdgWmBase, &listener, nullptr);
        }
    };
    auto registryRemover = [](void *data, wl_registry *registry, u32 id) {};
    const wl_registry_listener registryListener = {.global = registryHandler, .global_remove = registryRemover};
    wl_registry_add_listener(Registry, &registryListener, nullptr);
    wl_display_roundtrip(Display);

    SSSEngine::Platform::RunApplication(argc, argv);

    wl_display_disconnect(Display);
    return 0;
}
