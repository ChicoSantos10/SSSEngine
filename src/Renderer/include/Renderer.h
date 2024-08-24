//
// Created by Francisco Santos on 17/07/2023.
//

#pragma once

#include "Window.h"

namespace SSSRenderer
{
	typedef void (*CreateSwapChain_t)(const SSSEngine::Window &window);
	typedef void (*Render_t)();
	typedef void (*Terminate_t)();
	typedef void (*LoadAssetsTest_t)();
	typedef void (*ResizeSwapChain_t)(uint32_t width, uint32_t height);

	inline CreateSwapChain_t CreateSwapChain;
	inline Render_t Render;
	inline Terminate_t Terminate;
	inline LoadAssetsTest_t LoadAssetsTest;
	inline ResizeSwapChain_t ResizeSwapChain;

	void LoadDirectx();
	void Unload();
}
