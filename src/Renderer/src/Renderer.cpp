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

    SSSENGINE_INTERNAL void *Module;

    void Unload()
    {
        Terminate();
        SSSEngine::UnloadSharedLibrary(Module);
        Module = nullptr;
    }

    void LoadDirectx()
    {
        if(Module)
            Unload();

        constexpr auto RelativePath = LR"(Directx12\Directx12.dll)";
        Module = SSSEngine::LoadSharedLibrary(RelativePath, 0);

        // TODO: Proper handling / exception throwing
        if(!Module)
            throw std::exception();

        SSSEngine::LoadFunction<Init_t>(Module, "Initialize")();
        LoadAssetsTest = SSSEngine::LoadFunction<LoadAssetsTest_t>(Module, "LoadAssetsTest");
        CreateSwapChain = SSSEngine::LoadFunction<CreateSwapChain_t>(Module, "CreateSwapChain");
        ResizeSwapChain = SSSEngine::LoadFunction<ResizeSwapChain_t>(Module, "ResizeSwapChain");
        Render = SSSEngine::LoadFunction<Render_t>(Module, "Render");
        Terminate = SSSEngine::LoadFunction<Terminate_t>(Module, "Terminate");
    }
} // namespace SSSRenderer
