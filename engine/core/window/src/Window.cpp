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

#include "Window.h"
#include "Renderer.h"
#include "WindowHandle.h"

namespace SSSEngine::Core
{
    Window::Window(const Platform::WindowVec position, const Platform::WindowVec size,
                   const Platform::WindowTitle &title, const Window *parent) :
    m_handle{Platform::OpenWindow(position, size, title, parent ? parent->m_handle : nullptr)}
    {
        Renderer::CreateSwapChain(m_handle);
    }

    Window::~Window()
    {
        // INVESTIGATE: Should we release the swap chain?
        // SSSRenderer::ReleaseSwapChain();
    }
} // namespace SSSEngine::Core
