//
// Created by Francisco Santos on 17/07/2023.
//

#pragma once

namespace Renderer
{
	typedef void (* CreateSwapChain_t)(HWND);
	typedef void (* CreateRTV_t)();
	typedef void (* Render_t)();
	typedef void (* Terminate_t)();
	typedef void (* LoadAssetsTest_t)();
	typedef void (* CreateDepthStencilBuffer_t)(uint32_t width, uint32_t height);

	inline CreateSwapChain_t CreateSwapChain;
	inline CreateRTV_t CreateRtv;
	inline Render_t Render;
	inline Terminate_t Terminate;
	inline LoadAssetsTest_t LoadAssetsTest;
	inline CreateDepthStencilBuffer_t CreateDepthStencilBuffer;

	void LoadDirectx();
	void Unload();
}
