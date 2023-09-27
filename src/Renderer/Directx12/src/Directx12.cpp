//
// Created by Francisco Santos on 20/09/2023.
//

#include <windows.h>
#include "Exceptions.h"
#include "d3d12.h"
#include "d3dx12.h"
#include "dxguids.h"
#include "d3dcompiler.h"
#include "DirectXMath.h"
#include "comdef.h"
#include "dxgi1_6.h"
#include "Win32Utils.h"

#include <iostream> // REMOVE: Don't use this and instead create a function that receives a callback function?

//extern "C" { __declspec(dllexport) extern const UINT D3D12SDKVersion = 610; }
//extern "C" { __declspec(dllexport) extern const char8_t* D3D12SDKPath = u8".\\D3D12\\"; }

#define SSSENGINE_DLL_EXPORT extern "C" __declspec(dllexport)

namespace Directx12
{
	namespace
	{
		using namespace Win32;

		constexpr int BackBuffersAmount = 3;

		UINT descriptorSize;
		uint64_t frameFenceValues[BackBuffersAmount];

		ComPtr<IDXGIFactory6> factory;
		ComPtr<ID3D12Device9> device;
		ComPtr<ID3D12CommandQueue> commandQueue;
		ComPtr<ID3D12GraphicsCommandList> commandList;
		ComPtr<IDXGISwapChain4> swapChain;
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		ComPtr<ID3D12Resource> backBuffers[BackBuffersAmount];
		ComPtr<ID3D12CommandAllocator> commandAllocators[BackBuffersAmount];
		ComPtr<ID3D12Fence> fence;
		HANDLE fenceEvent;

		ComPtr<ID3D12InfoQueue1> infoQueue;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle()
	{
#if defined(_MSC_VER) || !defined(_WIN32)
		return descriptorHeap->GetCPUDescriptorHandleForHeapStart();
#else
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		descriptorHeap->GetCPUDescriptorHandleForHeapStart(&handle);
		return handle;
#endif
	}

	void Signal()
	{
		auto index = swapChain->GetCurrentBackBufferIndex();
		ThrowIfFailed(commandQueue->Signal(fence.Get(), ++frameFenceValues[index]));
	}

	void WaitForFenceValue()
	{
		auto index = swapChain->GetCurrentBackBufferIndex();
		auto value = frameFenceValues[index];
		if (fence->GetCompletedValue() < value)
		{
			ThrowIfFailed(fence->SetEventOnCompletion(value, fenceEvent));
			WaitForSingleObject(fenceEvent, INFINITE);
		}
	}

	void Flush()
	{
		Signal();
		WaitForFenceValue();
	}

	SSSENGINE_DLL_EXPORT D3D_FEATURE_LEVEL FindMaxFeatureLevel()
	{
		constexpr D3D_FEATURE_LEVEL features[] =
				{
						D3D_FEATURE_LEVEL_11_0,
						D3D_FEATURE_LEVEL_11_1,
						D3D_FEATURE_LEVEL_12_0,
						D3D_FEATURE_LEVEL_12_1,
						D3D_FEATURE_LEVEL_12_2,
				};

		D3D12_FEATURE_DATA_FEATURE_LEVELS levels
				{_countof(features), features, features[0]};


		device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &levels, sizeof(levels));

		return levels.MaxSupportedFeatureLevel;
	}

	SSSENGINE_DLL_EXPORT void Initialize()
	{
		using namespace Win32;

		// Enabling debug
		{
#if SSSENGINE_DEBUG_GRAPHICS
			ComPtr<ID3D12Debug> debugInterface;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
			{
				debugInterface->EnableDebugLayer();
			}
#endif
		}

		// Creating Device
		{
			ComPtr<IDXGIFactory2> factory2;
			UINT factoryFlags = 0;

#if SSSENGINE_DEBUG_GRAPHICS
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

			ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory2)));
			ThrowIfFailed(factory2.As(&factory));

			ComPtr<IDXGIAdapter4> adapter;
			ThrowIfFailed(factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)));
			ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device)));
		}

#if 0 // TODO: Implement info queue
		// Create Info Queue
		{
			ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&infoQueue)));
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

			std::cout << "Creating Info Queue" << std::endl;

			D3D12_MESSAGE_SEVERITY severities[] =
					{
							D3D12_MESSAGE_SEVERITY_INFO,
							D3D12_MESSAGE_SEVERITY_WARNING,
							D3D12_MESSAGE_SEVERITY_ERROR,
							D3D12_MESSAGE_SEVERITY_CORRUPTION,
							D3D12_MESSAGE_SEVERITY_MESSAGE,
					};

			D3D12_MESSAGE_ID denyIds[] =
					{
							D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
							D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
							D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
					};

			D3D12_INFO_QUEUE_FILTER newFilter = {};
			newFilter.AllowList.NumSeverities = _countof(severities);
			newFilter.AllowList.pSeverityList = severities;
			newFilter.DenyList.NumSeverities = 0;
			newFilter.DenyList.pSeverityList = nullptr;
			newFilter.DenyList.NumIDs = _countof(denyIds);
			newFilter.DenyList.pIDList = denyIds;

			infoQueue->PushStorageFilter(&newFilter);

			DWORD messageCallbackCookie;
			auto messageCallback =
					[](D3D12_MESSAGE_CATEGORY cat, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, const char* description,
					   void* context)
					{
						std::cout << "Directx " << cat << "ID: " << id << " ---> " << description << std::endl;
						system("pause");
					};
			ThrowIfFailed(infoQueue->RegisterMessageCallback(
					messageCallback,
					D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS,
					nullptr,
					&messageCallbackCookie
			));
		}
#endif

		// Command Queue
		{
			D3D12_COMMAND_QUEUE_DESC desc;
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.NodeMask = 0;

			ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)));
		}

		// Create descriptor heap
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = BackBuffersAmount;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 0;

			ThrowIfFailed(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap)));
		}

		// Create command allocator
		{
			for (auto& commandAllocator: commandAllocators)
			{
				ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
			}
		}

		// Create Command List
		{
			ThrowIfFailed(
					device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&commandList)));

			ThrowIfFailed(commandList->Close());
		}

		// Create fence
		{
			ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		}
	}

	SSSENGINE_DLL_EXPORT void CreateSwapChain(HWND window)
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = 1920;
		desc.Height = 1080;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = false;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = BackBuffersAmount;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = 0; // TODO: Check appropriate flags like allow tearing etc.

		ComPtr<IDXGISwapChain1> chain;
		ThrowIfFailed(factory->CreateSwapChainForHwnd(commandQueue.Get(), window, &desc, nullptr, nullptr, &chain));
		ThrowIfFailed(chain.As(&swapChain));
	}

	SSSENGINE_DLL_EXPORT void CreateRTV()
	{
		descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetDescriptorHandle();

		for (UINT i = 0; i < BackBuffersAmount; ++i)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
			ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

			device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
			backBuffers[i] = backBuffer;
			rtvHandle.ptr += descriptorSize;
		}
	}

	SSSENGINE_DLL_EXPORT void Render()
	{
		auto backBufferIndex = swapChain->GetCurrentBackBufferIndex();
		auto commandAllocator = commandAllocators[backBufferIndex];
		auto backBuffer = backBuffers[backBufferIndex];

		// Populate command list
		{
			ThrowIfFailed(commandAllocator->Reset());
			ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));

			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			commandList->ResourceBarrier(1, &barrier);

			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(GetDescriptorHandle(), static_cast<int>(backBufferIndex), descriptorSize);
			commandList->OMSetRenderTargets(1, &handle, false, nullptr);

			constexpr float clearColor[]{0.5f, 0.5f, 0.75f, 1.0f};
			commandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);

			barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			commandList->ResourceBarrier(1, &barrier);

			ThrowIfFailed(commandList->Close());
		}

		ID3D12CommandList* commandLists[] = {commandList.Get()};
		commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		swapChain->Present(1, 0);

		Flush();
	}

	SSSENGINE_DLL_EXPORT void Terminate()
	{
		Flush();
		CloseHandle(fenceEvent);

		//factory->Release();
		//device->Release();
		//commandQueue->Release();
		//commandList->Release();
		swapChain->Release(); // TODO: Figure out why this needs to be manually released. May have to do with the backing buffers keeping a reference
		//descriptorHeap->Release();
		//fence->Release();
	}
}

BOOL WINAPI DllMain(HMODULE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	using namespace Directx12;

	switch (fdwReason)
	{
		case DLL_PROCESS_ATTACH: break;

		case DLL_THREAD_ATTACH:
			// Do thread-specific initialization.
			break;

		case DLL_THREAD_DETACH:
			// Do thread-specific cleanup.
			break;

		case DLL_PROCESS_DETACH:

			if (lpvReserved != nullptr)
			{
				break; // do not do cleanup if process termination scenario
			}

			// Perform any necessary cleanup.
			break;
		default: break;
	}

	return true;
}