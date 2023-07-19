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
		Renderer();
		void Initialize(HWND window);
		void EnableDebugLayer();
		void CreateInfoQueue();
		void CreateDevice();
		void CreateCommandQueue();
		void CreateSwapChain(HWND window);
		void CreateDescriptorHeaps();
		void CreateRenderTargetViews();

		[[nodiscard]] inline CommandQueue GetCommandQueue() const { return *m_CommandQueue; }
		[[nodiscard]] inline Device GetDevice() const { return *m_Device; }

		void SetClearColor(float r, float g, float b, float a, ID3D12GraphicsCommandList* commandList) const;
		inline void Flush();
	private:
		static const uint16_t m_BackBuffersAmount = 3;
		std::unique_ptr<Device> m_Device;
		std::unique_ptr<CommandQueue> m_CommandQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain; // TODO: Create classes for each of these possibly
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_BackBuffers[m_BackBuffersAmount];
		UINT m_RtvDescriptorSize{};

		[[nodiscard]] D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const;
	};

} // Directx
