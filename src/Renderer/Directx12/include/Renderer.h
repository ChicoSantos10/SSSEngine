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

		void SetClearColor(float r, float g, float b, float a, ID3D12GraphicsCommandList* commandList) const;
	private:
		static const uint16_t m_BackBuffersAmount = 3;
		Device m_Device;
		CommandQueue m_CommandQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain; // TODO: Create classes for each of these possibly
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_BackBuffers[m_BackBuffersAmount];
		UINT m_RtvDescriptorSize;

		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const;
	};

} // Directx
