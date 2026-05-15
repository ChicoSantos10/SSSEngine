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

#include "Application.h"

#include "Allocator.h"
#include "Debug.h"
#include "Platform.h"
#include "Audio.h"
#include "Renderer.h"
#include "String.h"
#include "Timer.h"
#include "Input.h"
#include "Window.h"
#include "WindowHandle.h"

#include <iostream>

namespace SSSEngine::Editor
{
    Application::Application() :
        // TODO: Manage memory
        m_Window(reinterpret_cast<Core::Window *>(
            Memory::Allocators->Allocate(Math::Bytes(sizeof(Core::Window)), alignof(Core::Window))))
    {
        // TODO: Allow user to define the renderer to use
        Renderer::LoadRenderer(Renderer::Renderer::Vulkan);
        Audio::Init();
    }

    void Application::Run()
    {
        SSSENGINE_ASSERT(!m_Running);
        m_Running = true;

        Renderer::LoadAssetsTest();
        auto firstTimestamp = Platform::GetCurrentTime();
        while(m_Running)
        {
            m_Running = Input::HandleInput();

            // Render
            {
                try
                {
                    Renderer::BeginFrame();
                    Renderer::Render();
                }
                catch(std::exception &e)
                {
                    std::cerr << e.what() << "\n";
                    SSSENGINE_DEBUG_BREAK;
                    break;
                }
            }

            auto lastTimestamp = Platform::GetCurrentTime();
            auto deltaTime = lastTimestamp - firstTimestamp;
            SSSENGINE_ASSERT(deltaTime.value > 0);
            firstTimestamp = lastTimestamp;
            // SSSENGINE_LOG_INFO("Elapsed Microseconds: {}", deltaTime);
        }
    }
} // namespace SSSEngine::Editor

// TODO: Pass Array of Ascii strings instead
void SSSEngine::Platform::RunApplication(int argc, char *argv[])
{
    Editor::Application app;
    app.Run();
}
