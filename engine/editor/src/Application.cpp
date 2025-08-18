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

/**
 * @file
 * @brief
 */

#include <iostream>
#include <memory>

#include "Application.h"
#include "Debug.h"
#include "Platform.h"
#include "Audio.h"
#include "Timer.h"
#include "Input.h"
#include "WindowHandle.h"

namespace SSSEngine::Editor
{
    Application::Application()
    {
        Renderer::LoadDirectx();
        Audio::Init();

        // TODO: Manage memory
        m_Window = std::make_unique<Core::Window>(
            Platform::WindowVec{0, 0}, Platform::WindowVec{3440, 1440}, Platform::MainWindowName);
    }

    void Application::Run()
    {
        SSSENGINE_ASSERT(!m_Running);
        m_Running = true;

        Renderer::LoadAssetsTest();
        Platform::Timestamp firstTimestamp = Platform::GetCurrentTime();
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

            Platform::Timestamp lastTimestamp = Platform::GetCurrentTime();
            u64 elapsedMicroseconds = Platform::ToMicroSeconds(lastTimestamp - firstTimestamp);
            SSSENGINE_ASSERT(elapsedMicroseconds > 0);
            firstTimestamp = lastTimestamp;
            // SSSENGINE_LOG_INFO("Elapsed Microseconds: {}", elapsedMicroseconds);
        }
    }

    //	}
} // namespace SSSEngine::Editor
