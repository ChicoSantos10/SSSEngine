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

#include "Renderer.h"
#include "Library.h"

namespace SSSEngine::Renderer
{
    using Init_t = void (*)();

    SSSENGINE_INTERNAL void *Module;

    void Unload()
    {
        Terminate();
        Platform::UnloadSharedLibrary(Module);
        Module = nullptr;
    }

    void LoadDirectx()
    {
        if(Module)
            Unload();

        constexpr auto RelativePath = LR"(Directx12\Directx12.dll)";
        Module = Platform::LoadSharedLibrary(RelativePath, 0);

        // TODO: Proper handling / exception throwing
        if(!Module)
            throw std::exception();

        Platform::LoadFunction<Init_t>(Module, "Initialize")();
        LoadAssetsTest = Platform::LoadFunction<LoadAssetsTest_t>(Module, "LoadAssetsTest");
        BeginFrame = Platform::LoadFunction<BeginFrame_t>(Module, "BeginFrame");
        CreateSwapChain = Platform::LoadFunction<CreateSwapChain_t>(Module, "CreateSwapChain");
        ResizeSwapChain = Platform::LoadFunction<ResizeSwapChain_t>(Module, "ResizeSwapChain");
        Render = Platform::LoadFunction<Render_t>(Module, "Render");
        Terminate = Platform::LoadFunction<Terminate_t>(Module, "Terminate");
    }
} // namespace SSSEngine::Renderer
