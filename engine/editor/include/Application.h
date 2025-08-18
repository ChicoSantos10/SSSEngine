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

#include <memory>

#include "Renderer.h"
#include "Window.h"

namespace SSSEngine::Editor
{
    // INVESTIGATE: Does it make sense to be a class?
    /**
     * @class Application
     * @brief The editor application
     *
     */
    class Application final
    {
        public:
        Application();
        Application(const Application &) = delete;
        Application(Application &&) = delete;
        Application &operator=(const Application &) = delete;
        Application &operator=(Application &&) = delete;

        ~Application()
        {
            Renderer::Unload();
        };

        void Run();

        private:
        std::unique_ptr<Core::Window> m_Window;
        bool m_Running = false;
    };

} // namespace SSSEngine::Editor
