//
// Created by Francisco Santos on 17/07/2023.
//

#include "Device.h"
#include "dxgi1_6.h"
#include "Win32Utils.h"

namespace Renderer::Directx
{
	Device::Device()
	{
		Microsoft::WRL::ComPtr<IDXGIFactory2> factory2;
		Microsoft::WRL::ComPtr<IDXGIFactory4> factory4;
		UINT factoryFlags = 0;

#if DEBUG
		factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

		Win32::ThrowIfFailed(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory2)));
		Win32::ThrowIfFailed(factory2.As(&factory4));

		Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter1;
		Microsoft::WRL::ComPtr<IDXGIAdapter4> hardwareAdapter4;

		bool useWarp = false; // TODO: Make this a parameter
		if (useWarp)
		{
			Win32::ThrowIfFailed(factory4->EnumWarpAdapter(IID_PPV_ARGS(&hardwareAdapter1)));
			Win32::ThrowIfFailed(hardwareAdapter1.As(&hardwareAdapter4));
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
					Win32::ThrowIfFailed(hardwareAdapter1.As(&hardwareAdapter4));
				}
			}
		}

		// Create device
		Win32::ThrowIfFailed(D3D12CreateDevice(hardwareAdapter4.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_Device)));
	}
} // Directx