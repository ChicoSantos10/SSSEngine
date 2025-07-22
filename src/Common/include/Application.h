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

#include <memory>

#include "Renderer.h"
#include "Window.h"

namespace SSSEngine
{
    // INVESTIGATE: Does it make sense to be a class?
    class Application final
    {
        public:
        Application();
        Application(const Application &) = delete;
        Application(Application &&) = delete;
        Application &operator=(const Application &) = delete;
        Application &operator=(Application &&) = delete;
        ~Application() { SSSRenderer::Unload(); };

        void Run();

        private:
        std::unique_ptr<Window> m_Window;
        bool m_Running = false;
    };

} // namespace SSSEngine
