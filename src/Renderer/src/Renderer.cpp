/*  SSS Engine
    Copyright (C) ${YEAR}  Francisco Santos

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

#include "Renderer.h"
#include "Library.h"

namespace SSSRenderer
{
    using Init_t = void (*)();

    SSSENGINE_INTERNAL void *module;

    void Unload()
    {
        Terminate();
        SSSEngine::UnloadSharedLibrary(module);
        module = nullptr;
    }

    void LoadDirectx()
    {
        if(module)
            Unload();

        constexpr auto relativePath = LR"(Directx12\Directx12.dll)";
        module = SSSEngine::LoadSharedLibrary(relativePath, 0);

        // TODO: Proper handling / exception throwing
        if(!module)
            throw std::exception();

        SSSEngine::LoadFunction<Init_t>(module, "Initialize")();
        LoadAssetsTest = SSSEngine::LoadFunction<LoadAssetsTest_t>(module, "LoadAssetsTest");
        CreateSwapChain = SSSEngine::LoadFunction<CreateSwapChain_t>(module, "CreateSwapChain");
        ResizeSwapChain = SSSEngine::LoadFunction<ResizeSwapChain_t>(module, "ResizeSwapChain");
        Render = SSSEngine::LoadFunction<Render_t>(module, "Render");
        Terminate = SSSEngine::LoadFunction<Terminate_t>(module, "Terminate");
    }
} // namespace SSSRenderer
