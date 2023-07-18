//
// Created by Francisco Santos on 11/07/2023.
//

#include "CommandQueue.h"

#include <utility>
#ifdef __GNUC__
#include <cassert>
#endif
#include "Win32Utils.h"

// TODO: Create macro for not implemented exception and a new exception class

namespace Renderer::Directx
{
#pragma region Constructors

	CommandQueue::CommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type) : m_Device(std::move(device)), m_CommandListType(type), m_FenceValue(0)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.NodeMask = 0;

		Win32::ThrowIfFailed(m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CommandQueue)));
		Win32::ThrowIfFailed(m_Device->CreateFence(m_FenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));

		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		assert(m_FenceEvent && "Failed to create fence event handle.");
	}

	CommandQueue::~CommandQueue() = default;

#pragma endregion

#pragma region Public

	ComPtr<ID3D12CommandQueue> CommandQueue::GetCommandQueue() const
	{
		return m_CommandQueue;
	}

	ComPtr<ID3D12GraphicsCommandList2> CommandQueue::GetCommandList()
	{
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		ComPtr<ID3D12GraphicsCommandList2> commandList;

		if (!m_CommandAllocators.empty() && IsFenceComplete(m_CommandAllocators.front().fenceValue))
		{
			commandAllocator = m_CommandAllocators.front().commandAllocator;
			m_CommandAllocators.pop();
		}
		else
		{
			commandAllocator = CreateCommandAllocator();
		}

		if (!m_CommandLists.empty())
		{
			commandList = m_CommandLists.front();
			m_CommandLists.pop();

			Win32::ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));
		}
		else
		{
			commandList = CreateCommandList(commandAllocator);
		}

		Win32::ThrowIfFailed(commandList->SetPrivateDataInterface(__uuidof(ID3D12CommandAllocator), commandAllocator.Get()));

		return commandList;
	}

	uint64_t CommandQueue::ExecuteCommandList(const ComPtr<ID3D12GraphicsCommandList2>& commandList)
	{
		commandList->Close();

		ID3D12CommandAllocator* commandAllocator;
		UINT dataSize = sizeof(commandAllocator);
		Win32::ThrowIfFailed(commandList->GetPrivateData(__uuidof(ID3D12CommandAllocator), &dataSize, &commandAllocator));

		ID3D12CommandList* const commandLists[] = {
			commandList.Get()
		};

		m_CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		uint64_t fenceValue = Signal();
		m_CommandAllocators.emplace(CommandAllocatorEntry{fenceValue, commandAllocator });

		m_CommandLists.push(commandList);

		commandAllocator->Release();

		return fenceValue;
	}

	uint64_t CommandQueue::Signal()
	{
		uint64_t fenceValueForSignal = ++m_FenceValue;
		Win32::ThrowIfFailed(m_CommandQueue->Signal(m_Fence.Get(), fenceValueForSignal));

		return fenceValueForSignal;
	}

	void CommandQueue::Flush()
	{
		uint64_t fenceValueForSignal = Signal();
		WaitForFenceValue(fenceValueForSignal);
	}

	bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
	{
		return m_Fence->GetCompletedValue() >= fenceValue;
	}

	void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
	{
		if (!IsFenceComplete(fenceValue))
		{
			Win32::ThrowIfFailed(m_Fence->SetEventOnCompletion(fenceValue, m_FenceEvent));
			WaitForSingleObject(m_FenceEvent, INFINITE);
		}
	}

#pragma endregion

#pragma region Protected

	ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
	{
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		Win32::ThrowIfFailed(m_Device->CreateCommandAllocator(m_CommandListType, IID_PPV_ARGS(&commandAllocator)));

		return commandAllocator;
	}

	ComPtr<ID3D12GraphicsCommandList2> CommandQueue::CreateCommandList(const ComPtr<ID3D12CommandAllocator>& allocator)
	{
		ComPtr<ID3D12GraphicsCommandList2> commandList;
		Win32::ThrowIfFailed(m_Device->CreateCommandList(0, m_CommandListType, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

		return commandList;
	}

#pragma endregion
}