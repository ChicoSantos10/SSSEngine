//
// Created by Francisco Santos on 17/07/2023.
//

#pragma once

#include "Window.h"

namespace SSSRenderer
{
    using CreateSwapChain_t = SwapChain (*)(const SSSEngine::Window &);
    using Render_t = void (*)();
    using Terminate_t = void (*)();
    using LoadAssetsTest_t = void (*)();
    using ResizeSwapChain_t = void (*)(const SSSEngine::Window &);

    // ReSharper disable CppInconsistentNaming
    inline CreateSwapChain_t CreateSwapChain;
    inline Render_t Render;
    inline Terminate_t Terminate;
    inline LoadAssetsTest_t LoadAssetsTest;
    inline ResizeSwapChain_t ResizeSwapChain;
    // ReSharper restore CppInconsistentNaming

    void LoadDirectx();
    void Unload();
} // namespace SSSRenderer
