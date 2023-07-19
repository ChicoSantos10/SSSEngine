//
// Created by Francisco Santos on 17/07/2023.
//

#include "Renderer.h"
#include "Win32Utils.h"
#include "d3d12.h"

#include <iostream>

namespace Renderer::Directx
{
	void DebugMessageCallback(D3D12_MESSAGE_CATEGORY cat, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, const char* description,
	                          void* context)
	{
#ifdef SSSENGINE_DEBUG
		std::cout << "Directx " << cat << "ID: " << id << " ---> " << description << std::endl;
		system("pause");
#endif
	}

	void Renderer::EnableDebugLayer()
	{
#if SSSENGINE_DEBUG
		ComPtr<ID3D12Debug> debugInterface;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
		{
			debugInterface->EnableDebugLayer();
		}
#endif
	}

	void Renderer::CreateInfoQueue()
	{
		Microsoft::WRL::ComPtr<ID3D12InfoQueue1> infoQueue;

		if (SUCCEEDED(m_Device->GetDevice().As(&infoQueue)))
		{
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

			DWORD messageCallbackCookie;
			Win32::ThrowIfFailed(infoQueue->RegisterMessageCallback(
					DebugMessageCallback,
					D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS,
					nullptr,
					&messageCallbackCookie));
		}
	}

	Renderer::Renderer()
	{
	}

	void Renderer::Initialize(HWND window)
	{
		EnableDebugLayer(); // FIXME: Fix this: This removes the device so we need to create again
		CreateDevice();
		CreateInfoQueue();
		CreateCommandQueue();
		CreateSwapChain(window);
		CreateDescriptorHeaps();
		CreateRenderTargetViews();
	}

	void Renderer::CreateDevice()
	{
		m_Device = std::make_unique<Device>(Device());
	}

	void Renderer::CreateCommandQueue()
	{
		m_CommandQueue = std::make_unique<CommandQueue>(CommandQueue(m_Device->GetDevice(), D3D12_COMMAND_LIST_TYPE_DIRECT));
	}

	void Renderer::CreateSwapChain(HWND window)
	{
		Microsoft::WRL::ComPtr<IDXGIFactory2> tmpFactory;
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory;

		UINT swapChainFlags = 0;
#if SSSENGINE_DEBUG
		swapChainFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		Win32::ThrowIfFailed(CreateDXGIFactory2(swapChainFlags, IID_PPV_ARGS(&tmpFactory)));
		Win32::ThrowIfFailed(tmpFactory.As(&factory));

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = 1920;
		desc.Height = 1080;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = false;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = m_BackBuffersAmount;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = 0; // TODO: Check appropriate flags like allow tearing etc.

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;

		try
		{ ;
			Win32::ThrowIfFailed(
					factory->CreateSwapChainForHwnd(m_CommandQueue->GetCommandQueue().Get(), window, &desc, nullptr, nullptr, &swapChain1));
		}
		catch (const std::exception& e)
		{
			char reason[1024];
			auto device = m_Device->GetDevice();
			HRESULT hr = device->GetDeviceRemovedReason();
			if (FAILED(hr))
				Win32::GetErrorMessage(hr, reason);

			std::cout << reason << std::endl;
			throw std::exception();
		}
		Win32::ThrowIfFailed(swapChain1.As(&m_SwapChain));
	}

	void Renderer::CreateDescriptorHeaps()
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = m_BackBuffersAmount;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		Win32::ThrowIfFailed(m_Device->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_RtvDescriptorHeap)));
	}

	void Renderer::CreateRenderTargetViews()
	{
		m_RtvDescriptorSize = m_Device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		{
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetDescriptorHandle();

			for (UINT i = 0; i < m_BackBuffersAmount; ++i)
			{
				Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
				Win32::ThrowIfFailed(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)));

				m_Device->GetDevice()->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
				m_BackBuffers[i] = backBuffer;
				rtvHandle.ptr += m_RtvDescriptorSize;
			}
		}
	}

	void Renderer::SetClearColor(float r, float g, float b, float a, ID3D12GraphicsCommandList* commandList) const
	{
		float clearColor[] = {r, g, b, a};
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(GetDescriptorHandle(), m_SwapChain->GetCurrentBackBufferIndex(),
		                                                                        m_RtvDescriptorSize);
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	}

	void Renderer::Flush()
	{
		m_CommandQueue->Flush();
	}

	D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetDescriptorHandle() const
	{
#if defined(_MSC_VER) || !defined(_WIN32)
		return m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
#else
		D3D12_CPU_DESCRIPTOR_HANDLE handle;
		m_RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(&handle);
		return handle;
#endif
	}
} // Directx