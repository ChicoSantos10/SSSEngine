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

#include <iostream>
#include <memory>

#include "Application.h"
#include "Debug.h"
#include "Platform.h"
#include "Audio.h"
#include "Timer.h"
#include "Input.h"

namespace SSSEngine
{
    Application::Application()
    {
        SSSRenderer::LoadDirectx();
        SSSAudio::Init();

        // TODO: Manage memory
        m_Window = std::make_unique<Window>(0, 0, 1920, 1080, MainWindowName);
    }

    void Application::Run()
    {
        SSSENGINE_ASSERT(!m_Running);
        m_Running = true;

        SSSRenderer::LoadAssetsTest();
        SSSEngine::Timestamp firstTimestamp = SSSEngine::GetCurrentTime();
        while(m_Running)
        {
            m_Running = SSSInput::HandleInput();

            // Render
            {
                try
                {
                    SSSRenderer::BeginFrame();
                    SSSRenderer::Render();
                }
                catch(std::exception &e)
                {
                    std::cerr << e.what() << "\n";
                    SSSENGINE_DEBUG_BREAK;
                    break;
                }
            }

            SSSEngine::Timestamp lastTimestamp = SSSEngine::GetCurrentTime();
            u64 elapsedMicroseconds = SSSEngine::ToMicroSeconds(lastTimestamp - firstTimestamp);
            SSSENGINE_ASSERT(elapsedMicroseconds > 0);
            firstTimestamp = lastTimestamp;
            // SSSENGINE_LOG_INFO("Elapsed Microseconds: {}", elapsedMicroseconds);
        }

        // TODO: Handle events (keyboard, mouse, window, etc)

        // TODO: Update the application

        // TODO: Render the application
    }
    //	}
} // namespace SSSEngine
