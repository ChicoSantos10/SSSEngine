//
// Created by Francisco Santos on 04/07/2023.
//

#include <windows.h>
#include <iostream>
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "directx/dxguids/dxguids.h"
#include "dxgi1_6.h"
#include "d3dcompiler.h"
#include "DirectXMath.h"

#include <chrono>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace Renderer::DirectX
{
	const uint8_t backBuffersNumber = 3;
	bool useWarp = false;
	uint32_t width = 1280;
	uint32_t height = 720;
	bool isInitialized = false;

	RECT windowRect;
	Microsoft::WRL::ComPtr<ID3D12Device2> device;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource> backBuffers[backBuffersNumber];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocators[backBuffersNumber];
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	UINT rtvDescriptorSize;
	UINT currentBackBufferIndex;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	uint64_t fenceValue = 0;
	uint64_t frameFenceValues[backBuffersNumber] = {};
	HANDLE fenceEvent;

	void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw std::exception();
		}
	}

    D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle()
    {
#if defined(_MSC_VER) || !defined(_WIN32)
        return rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
#else
        D3D12_CPU_DESCRIPTOR_HANDLE handle;
        rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(&handle);
        return handle;
#endif
    }

	void InitializeDirectx12(HWND window)
	{
#if DEBUG
		// Enable the D3D12 debug layer.
		{
			Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
			{
				debugController->EnableDebugLayer();
			}
		}
#endif

		// Query adapter
		Microsoft::WRL::ComPtr<IDXGIFactory2> factory2;
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory4;
		UINT factoryFlags = 0;

#if DEBUG
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		if (!SUCCEEDED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory2))))
		{
			// Throw error
			throw std::runtime_error("Failed to create DXGI factory");
		}

        if (!SUCCEEDED(factory2.As(&factory4)))
        {
            // Throw error
            throw std::runtime_error("Failed to cast DXGI factory");
        }

		Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter1;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> hardwareAdapter4;

		if (useWarp)
		{
			if (!SUCCEEDED(factory4->EnumWarpAdapter(IID_PPV_ARGS(&hardwareAdapter1))))
			{
				// Throw error
				throw std::runtime_error("Failed to create warp adapter");
			}
			if (!SUCCEEDED(hardwareAdapter1.As(&hardwareAdapter4)))
			{
				// Throw error
				throw std::runtime_error("Failed to create warp adapter");
			}
		}
		else
		{
			SIZE_T maxDedicatedVideoMemory = 0;
			for (UINT i = 0; factory4->EnumAdapters1(i, &hardwareAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
			{
				DXGI_ADAPTER_DESC1 desc;
				hardwareAdapter1->GetDesc1(&desc);

				if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
				{
					// Don't select the Basic Render Driver adapter.
					continue;
				}

				if (desc.DedicatedVideoMemory > maxDedicatedVideoMemory)
				{
					maxDedicatedVideoMemory = desc.DedicatedVideoMemory;
					if (!SUCCEEDED(hardwareAdapter1.As(&hardwareAdapter4)))
					{
						// Throw error
						throw std::runtime_error("Failed to create hardware adapter");
					}
				}
			}
		}

		// Create device
		if (!SUCCEEDED(D3D12CreateDevice(hardwareAdapter4.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device))))
		{
			// Throw error
			throw std::runtime_error("Failed to create device");
		}

		// Enable debug messages
#if DEBUG
		Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;

		if (SUCCEEDED(device.As(&infoQueue)))
		{
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

			D3D12_MESSAGE_SEVERITY severities[] =
					{
							D3D12_MESSAGE_SEVERITY_INFO
					};

			D3D12_MESSAGE_ID denyIds[] =
					{
							D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
							D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
							D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
					};

			D3D12_INFO_QUEUE_FILTER newFilter = {};
			newFilter.DenyList.NumSeverities = _countof(severities);
			newFilter.DenyList.pSeverityList = severities;
			newFilter.DenyList.NumIDs = _countof(denyIds);
			newFilter.DenyList.pIDList = denyIds;

			infoQueue->PushStorageFilter(&newFilter);
		}
#endif

		// Create command queue
		{
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.NodeMask = 0;

			if (!SUCCEEDED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue))))
			{
				// Throw error
				throw std::runtime_error("Failed to create command queue");
			}
		}

		// Create swap chain
		Microsoft::WRL::ComPtr<IDXGIFactory7> factory7;

		UINT swapChainFlags = 0;
#if DEBUG
		swapChainFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		if (SUCCEEDED(CreateDXGIFactory2(swapChainFlags, IID_PPV_ARGS(&factory7))))
		{
			DXGI_SWAP_CHAIN_DESC1 desc = {};
			desc.Width = 1920;
			desc.Height = 1080;
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.Stereo = false;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			desc.BufferCount = backBuffersNumber;
			desc.Scaling = DXGI_SCALING_STRETCH;
			desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			desc.Flags = 0; // TODO: Check appropriate flags like allow tearing etc.

			Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
			if (!SUCCEEDED(factory7->CreateSwapChainForHwnd(commandQueue.Get(), window, &desc, nullptr, nullptr, &swapChain1)))
			{
				// Throw error
				throw std::runtime_error("Failed to create swap chain");
			}

			if (!SUCCEEDED(swapChain1.As(&swapChain)))
			{
				// Throw error
				throw std::runtime_error("Failed to create swap chain");
			}
		}

		// Create descriptor heap
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = backBuffersNumber;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 0;

			if (!SUCCEEDED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvDescriptorHeap))))
			{
				// Throw error
				throw std::runtime_error("Failed to create descriptor heap");
			}
		}

		// Create the render target view
		rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		{
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetDescriptorHandle();

			for (UINT i = 0; i < backBuffersNumber; ++i)
			{
				Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
				if (!SUCCEEDED(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
				{
					// Throw error
					throw std::runtime_error("Failed to get swap chain buffer");
				}

				device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
				backBuffers[i] = backBuffer;
				rtvHandle.ptr += rtvDescriptorSize;
			}
		}

		// Create command allocator
		if (!SUCCEEDED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocators[0]))))
		{
			// Throw error
			throw std::runtime_error("Failed to create command allocator");
		}

		// Create command list
		if (!SUCCEEDED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&commandList))))
		{
			// Throw error
			throw std::runtime_error("Failed to create command list");
		}

		// Create fence
		{
			if (!SUCCEEDED(device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
			{
				// Throw error
				throw std::runtime_error("Failed to create fence");
			}
		}

		// Create fence event
		fenceEvent = CreateEvent(nullptr, false, false, nullptr);
		if (fenceEvent == nullptr)
		{
			// Throw error
			throw std::runtime_error("Failed to create fence event");
		}
	}

	void Signal()
	{
		// Signal the fence event
		if (!SUCCEEDED(commandQueue->Signal(fence.Get(), ++fenceValue)))
		{
			// Throw error
			throw std::runtime_error("Failed to signal fence");
		}
	}

	void WaitForFenceValue(Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration = std::chrono::milliseconds::max() )
	{
		if (fence->GetCompletedValue() < fenceValue)
		{
			ThrowIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
			WaitForSingleObject(fenceEvent, static_cast<DWORD>(duration.count()));
		}
	}

	void Flush(Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue, Microsoft::WRL::ComPtr<ID3D12Fence> fence, uint64_t& fenceValue, HANDLE fenceEvent )
	{
		Signal();
		uint64_t fenceValueForSignal = fenceValue;
		WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
	}

	void Render()
	{
		auto commandAllocator = commandAllocators[currentBackBufferIndex];
		auto backBuffer = backBuffers[currentBackBufferIndex];

		commandAllocator->Reset();
		commandList->Reset(commandAllocator.Get(), nullptr);

		// Clear the render target
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			commandList->ResourceBarrier(1, &barrier);
		}

		float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(GetDescriptorHandle(), currentBackBufferIndex, rtvDescriptorSize);
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

		// Present
		{
			CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			commandList->ResourceBarrier(1, &barrier);

			commandList->Close();
		}

		// Execute the command list
		ID3D12CommandList* commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		// Present the frame
		if (!SUCCEEDED(swapChain->Present(1, 0)))
		{
			// Throw error
			throw std::runtime_error("Failed to present frame");
		}

		Signal();
		frameFenceValues[currentBackBufferIndex] = fenceValue;
	}
}

