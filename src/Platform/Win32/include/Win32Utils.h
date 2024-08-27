//
// Created by Francisco Santos on 11/07/2023.
//

#pragma once

#include <windows.h>
#include <wrl/client.h>
//#include "directx/d3d12.h"

#ifdef SSSENGINE_DEBUG
#define SSSENGINE_THROW_IF_FAILED(hr)  SSSENGINE_ASSERT(SUCCEEDED(hr))
#else
#define SSSENGINE_THROW_IF_FAILED(hr) ThrowIfFailed(hr)
#endif
namespace SSSWin32
{
	using namespace Microsoft::WRL;

	void ThrowIfFailed(HRESULT hr);
	void GetErrorMessage(HRESULT hr, char *message);
	//void GetDeviceRemovedReason(const ComPtr<ID3D12Device>&, char* message);
}
