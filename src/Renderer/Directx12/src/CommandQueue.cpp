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

	CommandQueue::CommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type) : m_device(std::move(device)), m_commandListType(type), m_fenceValue(0)
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = type;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.NodeMask = 0;

		Win32::ThrowIfFailed(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));
		Win32::ThrowIfFailed(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

		m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		assert(m_fenceEvent && "Failed to create fence event handle.");
	}

	CommandQueue::~CommandQueue() = default;

#pragma endregion

#pragma region Public

	ComPtr<ID3D12CommandQueue> CommandQueue::GetCommandQueue() const
	{
		return m_commandQueue;
	}

	ComPtr<ID3D12GraphicsCommandList2> CommandQueue::GetCommandList()
	{
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		ComPtr<ID3D12GraphicsCommandList2> commandList;

		if (!m_commandAllocators.empty() && IsFenceComplete(m_commandAllocators.front().fenceValue))
		{
			commandAllocator = m_commandAllocators.front().commandAllocator;
			m_commandAllocators.pop();
		}
		else
		{
			commandAllocator = CreateCommandAllocator();
		}

		if (!m_commandLists.empty())
		{
			commandList = m_commandLists.front();
			m_commandLists.pop();

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

		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		uint64_t fenceValue = Signal();
		m_commandAllocators.emplace(CommandAllocatorEntry{ fenceValue, commandAllocator });

		m_commandLists.push(commandList);

		commandAllocator->Release();

		return fenceValue;
	}

	uint64_t CommandQueue::Signal()
	{
		uint64_t fenceValueForSignal = ++m_fenceValue;
		Win32::ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), fenceValueForSignal));

		return fenceValueForSignal;
	}

	void CommandQueue::Flush()
	{
		uint64_t fenceValueForSignal = Signal();
		WaitForFenceValue(fenceValueForSignal);
	}

	bool CommandQueue::IsFenceComplete(uint64_t fenceValue)
	{
		return m_fence->GetCompletedValue() >= fenceValue;
	}

	void CommandQueue::WaitForFenceValue(uint64_t fenceValue)
	{
		if (!IsFenceComplete(fenceValue))
		{
			Win32::ThrowIfFailed(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent));
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}
	}

#pragma endregion

#pragma region Protected

	ComPtr<ID3D12CommandAllocator> CommandQueue::CreateCommandAllocator()
	{
		ComPtr<ID3D12CommandAllocator> commandAllocator;
		Win32::ThrowIfFailed(m_device->CreateCommandAllocator(m_commandListType, IID_PPV_ARGS(&commandAllocator)));

		return commandAllocator;
	}

	ComPtr<ID3D12GraphicsCommandList2> CommandQueue::CreateCommandList(const ComPtr<ID3D12CommandAllocator>& allocator)
	{
		ComPtr<ID3D12GraphicsCommandList2> commandList;
		Win32::ThrowIfFailed(m_device->CreateCommandList(0, m_commandListType, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));

		return commandList;
	}

#pragma endregion
}