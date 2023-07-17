//
// Created by Francisco Santos on 17/07/2023.
//

#pragma once

#include <windef.h>
#include "Device.h"
#include "CommandQueue.h"

namespace Renderer::Directx
{

	class Renderer
	{
	public:
		bool EnableDebugLayer();
		void CreateDevice();
		void CreateCommandQueue();
		void CreateSwapChain(HWND window);
		void CreateDescriptorHeaps();
		void CreateRenderTargetViews();

		void Render();
		void AttachWindow(HWND window);
	private:
		Device m_Device;
		CommandQueue m_CommandQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
		uint16_t m_BackBuffers = 3;
	};

} // Directx
