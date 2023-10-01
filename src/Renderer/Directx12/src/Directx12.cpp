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

#define SSSENGINE_DLL_EXPORT extern "C" [[maybe_unused]] __declspec(dllexport)

namespace Directx12
{
	namespace
	{
		using namespace Win32;

		constexpr int BackBuffersAmount = 3;

		UINT descriptorSize;
		uint64_t frameFenceValues[BackBuffersAmount];

		ComPtr<IDXGIFactory6> factory;
		ComPtr<ID3D12Device10> device;
		ComPtr<ID3D12CommandQueue> commandQueue;
		ComPtr<ID3D12GraphicsCommandList> commandList;
		ComPtr<IDXGISwapChain4> swapChain;
		ComPtr<ID3D12DescriptorHeap> descriptorHeap;
		ComPtr<ID3D12Resource> backBuffers[BackBuffersAmount];
		ComPtr<ID3D12CommandAllocator> commandAllocators[BackBuffersAmount];
		ComPtr<ID3D12Fence> fence;
		ComPtr<ID3D12RootSignature> rootSignature;
		ComPtr<ID3D12PipelineState> pipelineState;
		ComPtr<ID3D12InfoQueue> infoQueue;
		HANDLE fenceEvent;

		ComPtr<ID3D12Resource> vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

		D3D12_VIEWPORT viewport;
		D3D12_RECT scissorRect;
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
#ifdef SSSENGINE_DEBUG_GRAPHICS
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

#ifdef SSSENGINE_DEBUG_GRAPHICS
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

			ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory2)));
			ThrowIfFailed(factory2.As(&factory));

			ComPtr<IDXGIAdapter4> adapter;
			ThrowIfFailed(factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)));
			ThrowIfFailed(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device)));
		}

#ifdef SSSENGINE_DEBUG_GRAPHICS
		// Create Info Queue
		{
			ThrowIfFailed(device->QueryInterface(IID_PPV_ARGS(&infoQueue)));
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

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

		// Create fence
		{
			ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		}
	}

	SSSENGINE_DLL_EXPORT void CreateSwapChain(HWND window)
	{
		BOOL allowTearing = false;
		ThrowIfFailed(factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing)));

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = 0;  // Note: Passing 0 means use window height and width
		desc.Height = 0;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = false;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = BackBuffersAmount;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = allowTearing ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

		ComPtr<IDXGISwapChain1> chain;
		ThrowIfFailed(factory->CreateSwapChainForHwnd(commandQueue.Get(), window, &desc, nullptr, nullptr, &chain));
		ThrowIfFailed(chain.As(&swapChain));

		RECT rect;
		GetWindowRect(window, &rect);
		LONG width = rect.right - rect.left;
		LONG height = rect.bottom - rect.top;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<FLOAT>(width);
		viewport.Height = static_cast<FLOAT>(height);

		scissorRect.top = 0;
		scissorRect.left = 0;
		scissorRect.right = width;
		scissorRect.bottom = height;
	}

	SSSENGINE_DLL_EXPORT void CreateRTV()
	{
		descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(GetDescriptorHandle());

		for (UINT i = 0; i < BackBuffersAmount; ++i)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
			ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

			device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
			backBuffers[i] = backBuffer;
			rtvHandle.Offset(1, descriptorSize);
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
			ThrowIfFailed(commandList->Reset(commandAllocator.Get(), pipelineState.Get()));

//			commandList->SetPipelineState(pipelineState.Get());
			commandList->SetGraphicsRootSignature(rootSignature.Get());
			commandList->RSSetViewports(1, &viewport);
			commandList->RSSetScissorRects(1, &scissorRect);

			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
			commandList->ResourceBarrier(1, &barrier);

			CD3DX12_CPU_DESCRIPTOR_HANDLE handle(GetDescriptorHandle(), static_cast<int>(backBufferIndex), descriptorSize);
			commandList->OMSetRenderTargets(1, &handle, false, nullptr);

			constexpr float clearColor[]{0.5f, 0.5f, 0.75f, 1.0f};
			commandList->ClearRenderTargetView(handle, clearColor, 0, nullptr);
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			commandList->DrawInstanced(3, 1, 0, 0);

			barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
			commandList->ResourceBarrier(1, &barrier);

			ThrowIfFailed(commandList->Close());
		}

		ID3D12CommandList* commandLists[] = { commandList.Get() };
		commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		ThrowIfFailed(swapChain->Present(1, 0));

		Flush();
	}

	SSSENGINE_DLL_EXPORT void ResizeSwapChain(const ComPtr<IDXGISwapChain4>& chain, uint32_t width, uint32_t height)
	{
		assert(width > 0 && height > 0 && "Must pass appropriate values for width and height");

		DXGI_SWAP_CHAIN_DESC1 desc;
		chain->GetDesc1(&desc);
		if (desc.Width == width && desc.Height == height)
			return;

		// TODO: Needs to be specific to this swap chain
		Flush();
		for (int i = 0; i < BackBuffersAmount; ++i)
		{
			backBuffers[i].Reset();
			frameFenceValues[i] = frameFenceValues[chain->GetCurrentBackBufferIndex()];
		}

		ThrowIfFailed(chain->ResizeBuffers(BackBuffersAmount, width, height, desc.Format, desc.Flags));

		CreateRTV();
	}

	SSSENGINE_DLL_EXPORT void LoadAssetsTest()
	{
		// Root signature
		{
			CD3DX12_ROOT_SIGNATURE_DESC rootSig;
			rootSig.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;

			ThrowIfFailed(D3D12SerializeRootSignature(&rootSig, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
			ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
			                                          IID_PPV_ARGS(&rootSignature)));
		}

		// PSO
		{
			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> fragmentShader;
			ComPtr<ID3DBlob> errorMsgs;
			UINT compilerFlags = 0;

#ifdef SSSENGINE_DEBUG_GRAPHICS
			compilerFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
#endif

			auto DebugCompilation = [&](HRESULT hr)
			{
				if (FAILED(hr))
				{
					auto msg = reinterpret_cast<const char*>(errorMsgs->GetBufferPointer());
					std::cout << msg << std::endl;
				}
			};

			constexpr LPCWSTR filePath = LR"(N:\C++Projects\SSSEngine\src\Renderer\Shaders\TestShader.hlsl)";
			DebugCompilation(D3DCompileFromFile(filePath, nullptr, nullptr, "vertex", "vs_5_0", compilerFlags, 0, &vertexShader, &errorMsgs));
			DebugCompilation(D3DCompileFromFile(filePath, nullptr, nullptr, "fragment", "ps_5_0", compilerFlags, 0, &fragmentShader, &errorMsgs));

			D3D12_INPUT_ELEMENT_DESC inputDesc[]
			{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
					{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
			psoDesc.InputLayout = { inputDesc, _countof(inputDesc)};
			psoDesc.pRootSignature = rootSignature.Get();
			psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
			psoDesc.PS = CD3DX12_SHADER_BYTECODE(fragmentShader.Get());
			psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
			psoDesc.DepthStencilState.DepthEnable = false;
			psoDesc.DepthStencilState.StencilEnable = false;
			psoDesc.SampleMask = UINT_MAX;
			psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
			psoDesc.NumRenderTargets = 1;
			psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
			psoDesc.SampleDesc.Count = 1;

			ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
		}

		// Create Command List
		{
			ThrowIfFailed(
					device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), pipelineState.Get(), IID_PPV_ARGS(&commandList)));

			ThrowIfFailed(commandList->Close());
		}

		// Vertex Buffer
		{
			struct Vertex
			{
				DirectX::XMFLOAT3 position;
				DirectX::XMFLOAT4 color;
			};

			Vertex vertices[]
			{
				{ {0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f} },
				{ {0.25f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f} },
				{ {-0.25f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f} }
			};

			constexpr UINT vertexBufferSize = sizeof(vertices);
			CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
			auto desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

			ThrowIfFailed(device->CreateCommittedResource(
					&heapProperties,
					D3D12_HEAP_FLAG_NONE,
					&desc,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&vertexBuffer))
					);

			UINT8* begin;
			CD3DX12_RANGE readRange(0, 0);
			ThrowIfFailed(vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&begin)));
			memcpy(begin, vertices, vertexBufferSize);
			vertexBuffer->Unmap(0, nullptr);

			vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
			vertexBufferView.StrideInBytes = sizeof(Vertex);
			vertexBufferView.SizeInBytes = vertexBufferSize;
		}

		Flush();
	}

	SSSENGINE_DLL_EXPORT void Terminate()
	{
		Flush();
		CloseHandle(fenceEvent);
	}
}