//
// Created by Francisco Santos on 17/07/2023.
//

#include "Renderer.h"
#include "Win32Utils.h"

#include <iostream>

namespace Renderer::Directx
{
	void
	DebugMessageCallback(D3D12_MESSAGE_CATEGORY cat, D3D12_MESSAGE_SEVERITY severity, D3D12_MESSAGE_ID id, const char* description, void* context)
	{
		std::cout << "Directx " << cat << "ID: " << id << " ---> " << description << std::endl;
		system("pause");
	}

	bool Renderer::EnableDebugLayer()
	{
#if DEBUG
		ComPtr<ID3D12Debug> debugInterface;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
		{
			debugInterface->EnableDebugLayer();
		}
		else
			return false;

		Microsoft::WRL::ComPtr<ID3D12InfoQueue1> infoQueue;

		if (SUCCEEDED(m_Device.GetDevice().As(&infoQueue)))
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

			DWORD messageCallbackCookie;
			Win32::ThrowIfFailed(infoQueue->RegisterMessageCallback(
					DebugMessageCallback,
					D3D12_MESSAGE_CALLBACK_IGNORE_FILTERS,
					nullptr,
					&messageCallbackCookie));

			return true;
		}

		return false;
#endif
	}

	void Renderer::CreateDevice()
	{
		assert(false && "Not implemented");
	}

	void Renderer::CreateCommandQueue()
	{
		m_CommandQueue = CommandQueue(m_Device.GetDevice(), D3D12_COMMAND_LIST_TYPE_DIRECT);
	}

	void Renderer::CreateSwapChain(HWND window)
	{
		Microsoft::WRL::ComPtr<IDXGIFactory7> factory7;

		UINT swapChainFlags = 0;
#if DEBUG
		swapChainFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		Win32::ThrowIfFailed(CreateDXGIFactory2(swapChainFlags, IID_PPV_ARGS(&factory7)));

		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = 1920;
		desc.Height = 1080;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = false;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = m_BackBuffers;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = 0; // TODO: Check appropriate flags like allow tearing etc.

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;

		Win32::ThrowIfFailed(factory7->CreateSwapChainForHwnd(m_CommandQueue.GetCommandQueue().Get(), window, &desc, nullptr, nullptr, &swapChain1));
		Win32::ThrowIfFailed(swapChain1.As(&m_SwapChain));
	}

	void Renderer::CreateDescriptorHeaps()
	{
		assert(false && "Not implemented");
	}

	void Renderer::CreateRenderTargetViews()
	{
		assert(false && "Not implemented");
	}

	void Renderer::Render()
	{
		assert(false && "Not implemented");
	}

	void Renderer::AttachWindow(HWND window)
	{
		assert(false && "Not implemented");
	}
} // Directx