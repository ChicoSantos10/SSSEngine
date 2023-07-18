//
// Created by Francisco Santos on 11/07/2023.
//
#pragma once

#include <queue>

#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "directx/dxguids/dxguids.h"
#include "dxgi1_6.h"
#include "d3dcompiler.h"
#include "DirectXMath.h"
#include "comdef.h"

namespace Renderer::Directx
{
	using namespace Microsoft::WRL;

	class CommandQueue
	{
	public:
		CommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
		virtual ~CommandQueue();
		[[nodiscard]] ComPtr<ID3D12CommandQueue> GetCommandQueue() const;
		ComPtr<ID3D12GraphicsCommandList2> GetCommandList();
		uint64_t ExecuteCommandList(const ComPtr<ID3D12GraphicsCommandList2>& commandList);
		uint64_t Signal();
		void Flush();
		bool IsFenceComplete(uint64_t fenceValue);
		void WaitForFenceValue(uint64_t fenceValue);
	protected:
		ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
		ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(const ComPtr<ID3D12CommandAllocator>& allocator);
	private:
		struct CommandAllocatorEntry
		{
			uint64_t fenceValue;
			ComPtr<ID3D12CommandAllocator> commandAllocator;
		};

		ComPtr<ID3D12Device2> m_Device;
		ComPtr<ID3D12CommandQueue> m_CommandQueue;
		ComPtr<ID3D12Fence> m_Fence;
		std::queue<ComPtr<ID3D12GraphicsCommandList2>> m_CommandLists;
		std::queue<CommandAllocatorEntry> m_CommandAllocators;
		uint64_t m_FenceValue;
		D3D12_COMMAND_LIST_TYPE m_CommandListType;
		HANDLE m_FenceEvent;
	};
}