//
// Created by Francisco Santos on 14/07/2023.
//

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
