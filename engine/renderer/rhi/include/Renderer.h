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

#include "Platform.h"
#include "WindowHandle.h"
#include "SwapChainHandle.h"

namespace SSSEngine::Renderer
{
    using CreateSwapChain_t = SwapChainHandle (*)(const SSSEngine::Platform::WindowHandle &);
    using Render_t = void (*)();
    using Terminate_t = void (*)();
    using LoadAssetsTest_t = void (*)();
    using ResizeSwapChain_t = void (*)(const SSSEngine::Platform::WindowHandle &);
    using BeginFrame_t = void (*)();

    SSSENGINE_GLOBAL CreateSwapChain_t CreateSwapChain;
    SSSENGINE_GLOBAL Render_t Render;
    SSSENGINE_GLOBAL Terminate_t Terminate;
    SSSENGINE_GLOBAL LoadAssetsTest_t LoadAssetsTest;
    SSSENGINE_GLOBAL ResizeSwapChain_t ResizeSwapChain;
    SSSENGINE_GLOBAL BeginFrame_t BeginFrame;

    void LoadDirectx();
    void Unload();
} // namespace SSSEngine::Renderer
