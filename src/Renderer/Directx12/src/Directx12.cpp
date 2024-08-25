/*  SSS Engine
Copyright (C) 2024  Francisco Santos

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <windows.h>
#include "d3d12.h"
#include "d3dx12.h"
#include "DirectXMath.h"
#include "dxgi1_6.h"
#include "Win32Utils.h"
#include "dxcapi.h"
#include "Window.h"
#include "Attributes.h"
#include "Constants.h"

// TODO: LOG function/Macro for HR results

namespace SSSRenderer::SSSDirectx12
{
	// TODO: Set default values
	namespace
	{
		using namespace SSSWin32;

		// Constants
		constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		constexpr DXGI_FORMAT ClearValueFormat = DXGI_FORMAT_D32_FLOAT;

		// Descriptors
		ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
		ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
		UINT rtvDescriptorSize = 0;
		UINT dsvDescriptorSize = 0;
		UINT cbvSrvUavDescriptorSize = 0;

		// Fence
		ComPtr<ID3D12Fence> fence;
		uint64_t frameFenceValues[BackBuffersAmount];
		HANDLE fenceEvent;

		ComPtr<IDXGIFactory6> factory;
		ComPtr<ID3D12Device10> device;
		ComPtr<ID3D12CommandQueue> commandQueue;
		ComPtr<ID3D12GraphicsCommandList> commandList;
		ComPtr<ID3D12CommandAllocator> commandAllocators[BackBuffersAmount];
		ComPtr<ID3D12Resource> backBuffers[BackBuffersAmount];
		// TODO: For now 1 swap chain is enough but in the future we may want more so a list would perhaps be better
		//		However we would need to have a lot of different things for each such as command queues and lists...
		ComPtr<IDXGISwapChain4> swapChain;
		ComPtr<ID3D12RootSignature> rootSignature;
		ComPtr<ID3D12PipelineState> pipelineState;
		ComPtr<ID3D12InfoQueue> infoQueue;
		ComPtr<ID3D12Resource> depthStencilBuffer;

		ComPtr<ID3D12Resource> vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

		D3D12_VIEWPORT viewport;
		D3D12_RECT scissorRect;

		// AA
		UINT msaaMaxQualityLevelsSupported = 0;

		// Refresh Rate and Variable Refresh Rate support
		BOOL allowTearing = false;
	}

	void Signal()
	{
		const auto index = swapChain->GetCurrentBackBufferIndex();
		SSSENGINE_THROW_IF_FAILED(commandQueue->Signal(fence.Get(), ++frameFenceValues[index]));
	}

	void WaitForFenceValue()
	{
		const auto index = swapChain->GetCurrentBackBufferIndex();
		if (const auto value = frameFenceValues[index]; fence->GetCompletedValue() < value)
		{
			SSSENGINE_THROW_IF_FAILED(fence->SetEventOnCompletion(value, fenceEvent));
			WaitForSingleObject(fenceEvent, INFINITE);
		}
	}

	void Flush()
	{
		Signal();
		WaitForFenceValue();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> &descriptorHeap)
	{
#if defined(_MSC_VER) || !defined(_WIN32)
		return descriptorHeap->GetCPUDescriptorHandleForHeapStart();
#else
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		descriptorHeap->GetCPUDescriptorHandleForHeapStart(&handle);
		return handle;
#endif
	}

	void CreateRtv()
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(GetDescriptorHandle(rtvDescriptorHeap));

		for (UINT i = 0; i < BackBuffersAmount; ++i)
		{
			SSSENGINE_THROW_IF_FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])));

			device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(1, rtvDescriptorSize);
		}
	}

	void CreateDepthStencilBuffer(uint32_t width, uint32_t height)
	{
		SSSENGINE_ASSERT(width > 0 && height > 0);

		Flush();

		D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = ClearValueFormat;
		clearValue.DepthStencil = {1.0f, 0};

		// INVESTIGATE: Should we take into consideration the sample count even if we don't use it in the swap chain?
		CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(ClearValueFormat,
		                                                 width,
		                                                 height,
		                                                 1,
		                                                 0,
		                                                 1,
		                                                 0,
		                                                 D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
		);

		SSSENGINE_THROW_IF_FAILED(
			device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &resourceDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_PPV_ARGS(depthStencilBuffer.ReleaseAndGetAddressOf())
			)
		);

		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
		depthStencilDesc.Format = ClearValueFormat;
		depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
		depthStencilDesc.Texture2D.MipSlice = 0;
		depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

		device->CreateDepthStencilView(depthStencilBuffer.Get(),
		                               &depthStencilDesc,
		                               dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart()
		);
	}

	D3D_FEATURE_LEVEL FindMaxFeatureLevel()
	{
		// TODO: Check if this is correct
		constexpr D3D_FEATURE_LEVEL features[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_12_0,
			D3D_FEATURE_LEVEL_12_1,
			D3D_FEATURE_LEVEL_12_2,
		};

		D3D12_FEATURE_DATA_FEATURE_LEVELS levels{_countof(features), features, features[0]};

		device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &levels, sizeof(levels));

		return levels.MaxSupportedFeatureLevel;
	}

	SSSENGINE_DLL_EXPORT void Initialize()
	{
		using namespace SSSWin32;

#ifdef SSSENGINE_DEBUG_GRAPHICS
		// Enabling debug
		{
			ComPtr<ID3D12Debug> debugInterface;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
			{
				debugInterface->EnableDebugLayer();
			}
		}
#endif

		// Creating Device
		{
			ComPtr<IDXGIFactory2> factory2;
			UINT factoryFlags = 0;

#ifdef SSSENGINE_DEBUG_GRAPHICS
			factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

			SSSENGINE_THROW_IF_FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory2)));
			SSSENGINE_THROW_IF_FAILED(factory2.As(&factory));

			ComPtr<IDXGIAdapter4> adapter;
			SSSENGINE_THROW_IF_FAILED(
				factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter))
			);

			SSSENGINE_THROW_IF_FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&device)));
		}

#ifdef SSSENGINE_DEBUG_GRAPHICS
		// Create Info Queue
		{
			SSSENGINE_THROW_IF_FAILED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)));
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

			D3D12_MESSAGE_SEVERITY severities[] = {
				D3D12_MESSAGE_SEVERITY_INFO,
				D3D12_MESSAGE_SEVERITY_WARNING,
				D3D12_MESSAGE_SEVERITY_ERROR,
				D3D12_MESSAGE_SEVERITY_CORRUPTION,
				D3D12_MESSAGE_SEVERITY_MESSAGE,
			};

			D3D12_MESSAGE_ID denyIds[] = {
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

		// Root signature
		{
			// Create a root signature.
			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{D3D_ROOT_SIGNATURE_VERSION_1_1};
			if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			{
				// TODO: Maybe just log or SSSENGINE_ASSERT that it passes
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}

			// Allow input layout and deny unnecessary access to certain pipeline stages.
			constexpr D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

			CD3DX12_ROOT_PARAMETER1 rootParameters[1];
			rootParameters[0].InitAsConstants(sizeof(DirectX::XMMATRIX) * 0.25f, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

			CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSig;
			rootSig.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

			ComPtr<ID3DBlob> signature;
			ComPtr<ID3DBlob> error;

			SSSENGINE_THROW_IF_FAILED(
				D3DX12SerializeVersionedRootSignature(&rootSig, featureData.HighestVersion, &signature, &error)
			);
			SSSENGINE_THROW_IF_FAILED(
				device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&
					rootSignature) )
			);
		}

		// PSO
		{
			constexpr LPCWSTR filePath = LR"(N:\C++Projects\SSSEngine\src\Renderer\Shaders\TestShader.hlsl)";

			ComPtr<IDxcUtils> compilerUtils;
			ComPtr<IDxcCompiler3> shaderCompiler;
			DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&compilerUtils));
			DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&shaderCompiler));

			ComPtr<IDxcIncludeHandler> includeHandler;
			compilerUtils->CreateDefaultIncludeHandler(&includeHandler);

			LPCWSTR commandArgsVs[] = {
				L"TestShader.hlsl",
				// Optional shader src file
				L"-E",
				L"vertex",
				L"-T",
				L"vs_6_6",
				L"-Zs",
				L"-Qstrip_reflect",
				L"-Fo",
				L"TestShader.bin",
				L"-Fd",
				L"TestShader.pdb",
			};

			LPCWSTR commandArgsPs[] = {
				L"TestShader.hlsl",
				// Optional shader src file
				L"-E",
				L"fragment",
				L"-T",
				L"ps_6_6",
				L"-Zs",
				L"-Qstrip_reflect",
				L"-Fo",
				L"TestShader.bin",
				L"-Fd",
				L"TestShader.pdb",
			};

			ComPtr<IDxcBlobEncoding> encoder;
			compilerUtils->LoadFile(filePath, nullptr, &encoder);

			DxcBuffer buffer{};
			buffer.Ptr = encoder->GetBufferPointer();
			buffer.Size = encoder->GetBufferSize();
			buffer.Encoding = DXC_CP_ACP;

			auto debug = [](const ComPtr<IDxcResult> &result)
			{
				ComPtr<IDxcBlobUtf8> errors;
				result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
				if (errors && errors->GetStringLength() > 0)
				{
					std::cout << errors->GetStringPointer() << std::endl;
				}

				HRESULT hr;
				result->GetStatus(&hr);
				if (FAILED(hr))
				{
					std::cout << "Failed vertex shader compilation" << std::endl;
				}
			};

			ComPtr<IDxcResult> vsResult;
			ComPtr<IDxcResult> psResult;

			shaderCompiler->Compile(&buffer,
			                        commandArgsVs,
			                        _countof(commandArgsVs),
			                        includeHandler.Get(),
			                        IID_PPV_ARGS(&vsResult)
			);
			shaderCompiler->Compile(&buffer,
			                        commandArgsPs,
			                        _countof(commandArgsPs),
			                        includeHandler.Get(),
			                        IID_PPV_ARGS(&psResult)
			);

			debug(vsResult);
			debug(psResult);

			ComPtr<ID3DBlob> vertexShader;
			ComPtr<ID3DBlob> fragmentShader;

			vsResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&vertexShader), nullptr);
			psResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&fragmentShader), nullptr);

#ifdef SSSENGINE_FXC
			UINT compilerFlags = 0;
			ComPtr<ID3DBlob> errorMsgs;

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


			DebugCompilation(D3DCompileFromFile(filePath, nullptr, nullptr, "vertex", "vs_5_0", compilerFlags, 0, &vertexShader, &errorMsgs));
			DebugCompilation(D3DCompileFromFile(filePath, nullptr, nullptr, "fragment", "ps_5_0", compilerFlags, 0, &fragmentShader, &errorMsgs));
#endif

			D3D12_INPUT_ELEMENT_DESC inputDesc[]{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
				{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			};

			D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
			psoDesc.InputLayout = {inputDesc, _countof(inputDesc)};
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
			psoDesc.RTVFormats[0] = BackBufferFormat;
			psoDesc.SampleDesc.Count = 1;

			SSSENGINE_THROW_IF_FAILED(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
		}

		// Command Queue
		{
			D3D12_COMMAND_QUEUE_DESC desc;
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
			desc.NodeMask = 0;

			SSSENGINE_THROW_IF_FAILED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)));
		}

		// Create Command Allocators
		// TODO: We need a Command Allocator for each command list + for each thread (backBuffers * threads)
		{
			for (auto &commandAllocator : commandAllocators)
			{
				SSSENGINE_THROW_IF_FAILED(
					device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator) )
				);
			}
		}

		// Create Command List
		// TODO: We need a command list for each thread
		{
			// TODO: Maybe we can pass nullptr instead of the pipeline since we can define it on Reset?
			SSSENGINE_THROW_IF_FAILED(
				device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), pipelineState.
					Get(), IID_PPV_ARGS(&commandList) )
			);

			SSSENGINE_THROW_IF_FAILED(commandList->Close());
		}

		// Descriptor Sizes
		{
			rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			cbvSrvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}

		// Create RTV Descriptor Heap
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc = {};
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.NumDescriptors = BackBuffersAmount;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 0;

			SSSENGINE_THROW_IF_FAILED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvDescriptorHeap)));
		}

		// Create DSV Descriptor Heap
		{
			D3D12_DESCRIPTOR_HEAP_DESC dsvDesc{};
			dsvDesc.NumDescriptors = 1;
			dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			SSSENGINE_THROW_IF_FAILED(device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&dsvDescriptorHeap)));
		}

		// Create Fence
		{
			SSSENGINE_THROW_IF_FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		}

		// Anti Aliasing Support
		// TODO: Check all supported options
		// TODO: Implement Antialiasing in the render pipeline
		{
			// 4x MSAA
			D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels{
				.Format = BackBufferFormat,
				.SampleCount = 4,
				.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
				.NumQualityLevels = 0,
			};

			SSSENGINE_THROW_IF_FAILED(
				device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(
					qualityLevels) )
			);

			msaaMaxQualityLevelsSupported = qualityLevels.NumQualityLevels;
			SSSENGINE_ASSERT(msaaMaxQualityLevelsSupported > 0);
		}
	}

	// TODO: If we create another swap chain for a secondary window we replace the main one.
	//  Should we return the swap chain? Or store it in a set of HWND,SwapChain?
	//  Same thing for viewport and scissors rect
	SSSENGINE_DLL_EXPORT SwapChain CreateSwapChain(const SSSEngine::Window &window)
	{
		swapChain.Reset();

		SSSENGINE_THROW_IF_FAILED(
			factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing) )
		);

		HWND handle = static_cast<HWND>(window.GetHandle());

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = 0; // Note: Passing 0 means use window height and width
		desc.Height = 0;
		desc.Format = BackBufferFormat;
		desc.Stereo = false;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = BackBuffersAmount;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		if (allowTearing)
			flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
		desc.Flags = flags;

		ComPtr<IDXGISwapChain1> chain;
		SSSENGINE_THROW_IF_FAILED(
			factory->CreateSwapChainForHwnd(commandQueue.Get(), handle, &desc, nullptr, nullptr, &chain)
		);
		SSSENGINE_THROW_IF_FAILED(chain.As(&swapChain));

		SSSENGINE_THROW_IF_FAILED(factory->MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER));

		RECT rect;
		BOOL success = GetWindowRect(handle, &rect);
		SSSENGINE_ASSERT(success); // INVESTIGATE: Should we throw if failed in release versions?
		LONG width = rect.right - rect.left;
		LONG height = rect.bottom - rect.top;
		// INVESTIGATE: Should the viewport use 0,0 or the window position?
		viewport.TopLeftX = static_cast<FLOAT>(rect.left);
		viewport.TopLeftY = static_cast<FLOAT>(rect.top);
		viewport.Width = static_cast<FLOAT>(width);
		viewport.Height = static_cast<FLOAT>(height);

		scissorRect.top = 0;
		scissorRect.left = 0;
		scissorRect.right = width;
		scissorRect.bottom = height;

		CreateDepthStencilBuffer(width, height);
		CreateRtv();

		// TODO: Return the handle to the swap chain
		return {.handle = 0};
	}

	SSSENGINE_DLL_EXPORT void Render()
	{
		// Populate command list
		{
			const auto backBufferIndex = swapChain->GetCurrentBackBufferIndex();
			const auto commandAllocator = commandAllocators[backBufferIndex];
			const auto backBuffer = backBuffers[backBufferIndex];
			SSSENGINE_THROW_IF_FAILED(commandAllocator->Reset());
			SSSENGINE_THROW_IF_FAILED(commandList->Reset(commandAllocator.Get(), pipelineState.Get()));

			commandList->SetPipelineState(pipelineState.Get());
			commandList->SetGraphicsRootSignature(rootSignature.Get());
			commandList->RSSetViewports(1, &viewport);
			commandList->RSSetScissorRects(1, &scissorRect);

			auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(),
			                                                    D3D12_RESOURCE_STATE_PRESENT,
			                                                    D3D12_RESOURCE_STATE_RENDER_TARGET
			);
			commandList->ResourceBarrier(1, &barrier);

			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(GetDescriptorHandle(rtvDescriptorHeap),
			                                        static_cast<int>(backBufferIndex),
			                                        rtvDescriptorSize
			);
			CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(GetDescriptorHandle(dsvDescriptorHeap));
			commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

			constexpr float clearColor[]{0.5f, 0.5f, 0.75f, 1.0f};
			commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
			commandList->ClearDepthStencilView(dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
			                                   D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
			                                   1.0f,
			                                   0,
			                                   0,
			                                   nullptr
			);
			commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			commandList->DrawInstanced(3, 1, 0, 0);

			barrier = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(),
			                                               D3D12_RESOURCE_STATE_RENDER_TARGET,
			                                               D3D12_RESOURCE_STATE_PRESENT
			);
			commandList->ResourceBarrier(1, &barrier);

			SSSENGINE_THROW_IF_FAILED(commandList->Close());
		}

		ID3D12CommandList *commandLists[] = {commandList.Get()};
		commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		const auto renderFlags = allowTearing ? DXGI_PRESENT_ALLOW_TEARING : 0;
		// INVESTIGATE:
		//  - Should we use the Present1 function instead?
		//  - Best Sync Interval
		// TODO: Also check for windowed mode
		//  (Currently there is no way to set to exclusive fullscreen mode so not needed for now)
		if (allowTearing)
			SSSENGINE_THROW_IF_FAILED(swapChain->Present(0, renderFlags))
		else
			SSSENGINE_THROW_IF_FAILED(swapChain->Present(1, 0))

		Flush();
	}

	// TODO: Specify the swap chain
	SSSENGINE_DLL_EXPORT void ResizeSwapChain(const SSSEngine::Window &window)
	{
		HWND handle = static_cast<HWND>(window.GetHandle());
		RECT rect;
		GetWindowRect(handle, &rect);

		const auto width = rect.right - rect.left;
		const auto height = rect.bottom - rect.top;

		SSSENGINE_ASSERT(width > 0 && height > 0 && "Must pass appropriate values for width and height");

		DXGI_SWAP_CHAIN_DESC1 desc;
		swapChain->GetDesc1(&desc);
		if (desc.Width == width && desc.Height == height)
			return;

		// TODO: Needs to be specific to this swap chain
		Flush();
		for (int i = 0; i < BackBuffersAmount; ++i)
		{
			backBuffers[i].Reset();
			frameFenceValues[i] = frameFenceValues[swapChain->GetCurrentBackBufferIndex()];
		}

		SSSENGINE_THROW_IF_FAILED(swapChain->ResizeBuffers(BackBuffersAmount, width, height, desc.Format, desc.Flags));

		// TODO: Should this be it's own function? Since we usually call this 2 functions together
		CreateDepthStencilBuffer(width, height);
		CreateRtv();
	}

	// TODO: Remove this eventually
	SSSENGINE_DLL_EXPORT void LoadAssetsTest()
	{
		// Vertex Buffer
		{
			struct Vertex
			{
				DirectX::XMFLOAT3 position;
				DirectX::XMFLOAT4 color;
			};

			constexpr Vertex vertices[]{
				{{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
				{{0.25f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
				{{-0.25f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}
			};

			constexpr UINT vertexBufferSize = sizeof(vertices);
			CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
			auto desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);

			SSSENGINE_THROW_IF_FAILED(
				device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE, &desc,
					D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer) )
			);

			UINT8 *begin;
			CD3DX12_RANGE readRange(0, 0);
			SSSENGINE_THROW_IF_FAILED(vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&begin)));
			memcpy(begin, vertices, vertexBufferSize);
			vertexBuffer->Unmap(0, nullptr);

			vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
			vertexBufferView.StrideInBytes = sizeof(Vertex);
			vertexBufferView.SizeInBytes = vertexBufferSize;
		}

		// NOTE: No need to flush since this is loaded before we even commit drawing commands
		//		In the future we need to do this when commiting resources to the gpu
		//Flush();
	}

	SSSENGINE_DLL_EXPORT void Terminate()
	{
		Flush();
		CloseHandle(fenceEvent);
	}
}
