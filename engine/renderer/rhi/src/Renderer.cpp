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
#include "Attributes.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "Library.h"
#include "Path.h"
#include "StringView.h"

namespace SSSEngine::Renderer
{
    using Init_t = void (*)();

    SSSENGINE_INTERNAL Platform::LibraryHandle Module;

    void Unload()
    {
        Terminate();
        Platform::UnloadSharedLibrary(Module);
    }

    // TODO: Return a path instead

    /**
     * @brief Get's the path to the renderer dll
     *
     * @param renderer The renderer library to get the path of
     * @return The file path
     */
    SSSENGINE_FORCE_INLINE
    constexpr const char *GetRendererPath(Renderer renderer)
    {
        switch(renderer)
        {
#ifdef SSSENGINE_WIN32
            case Renderer::Directx12:
#endif // SSSENGINE_WIN32
            case Renderer::Vulkan:
                return R"(Vulkan/Vulkan.dll)";
        }
    }

    void LoadRenderer(Renderer renderer)
    {
        if(Module)
            Unload();

        // constexpr auto RelativePath = LR"(Directx12\Directx12.dll)";
        // TODO: Use path
        const char *relativePath = GetRendererPath(renderer);
        Module = Platform::LoadSharedLibrary(relativePath, 0);

        // TODO: Proper handling / exception throwing
        if(!Module)
        {
            // TODO: Handle Error
            SSSENGINE_ASSERT(false);
        }

        Platform::LoadFunction<Init_t>(Module, u8"Initialize")();
        LoadAssetsTest = Platform::LoadFunction<LoadAssetsTest_t>(Module, u8"LoadAssetsTest");
        BeginFrame = Platform::LoadFunction<BeginFrame_t>(Module, u8"BeginFrame");
        CreateSwapChain = Platform::LoadFunction<CreateSwapChain_t>(Module, u8"CreateSwapChain");
        ResizeSwapChain = Platform::LoadFunction<ResizeSwapChain_t>(Module, u8"ResizeSwapChain");
        Render = Platform::LoadFunction<Render_t>(Module, u8"Render");
        Terminate = Platform::LoadFunction<Terminate_t>(Module, u8"Terminate");
    }
} // namespace SSSEngine::Renderer
