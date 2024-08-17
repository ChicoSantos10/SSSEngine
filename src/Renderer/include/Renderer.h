//
// Created by Francisco Santos on 17/07/2023.
//

#pragma once
// TODO: This file is windows specific. Need to convert to generic renderer
namespace Renderer
{
	typedef void (* CreateSwapChain_t)(HWND window);
	typedef void (* Render_t)();
	typedef void (* Terminate_t)();
	typedef void (* LoadAssetsTest_t)();
	typedef void (* ResizeSwapChain_t)(uint32_t width, uint32_t height);

	inline CreateSwapChain_t CreateSwapChain;
	inline Render_t Render;
	inline Terminate_t Terminate;
	inline LoadAssetsTest_t LoadAssetsTest;
	inline ResizeSwapChain_t ResizeSwapChain;

	void LoadDirectx();
	void Unload();
}
