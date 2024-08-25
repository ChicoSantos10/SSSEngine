//
// Created by Francisco Santos on 17/07/2023.
//

#pragma once

#include "Window.h"

namespace SSSRenderer
{
	typedef SwapChain (*CreateSwapChain_t)(const SSSEngine::Window &window);
	typedef void (*Render_t)();
	typedef void (*Terminate_t)();
	typedef void (*LoadAssetsTest_t)();
	typedef void (*ResizeSwapChain_t)(const SSSEngine::Window &window);

	// ReSharper disable CppInconsistentNaming
	inline CreateSwapChain_t CreateSwapChain;
	inline Render_t Render;
	inline Terminate_t Terminate;
	inline LoadAssetsTest_t LoadAssetsTest;
	inline ResizeSwapChain_t ResizeSwapChain;
	// ReSharper restore CppInconsistentNaming

	void LoadDirectx();
	void Unload();
}
