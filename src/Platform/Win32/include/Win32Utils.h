//
// Created by Francisco Santos on 11/07/2023.
//

#pragma once

#include <windows.h>
#include <wrl/client.h>
//#include "directx/d3d12.h"

namespace SSSWin32
{
	using namespace Microsoft::WRL;

	void ThrowIfFailed(HRESULT hr);
	void GetErrorMessage(HRESULT hr, char* message);
	//void GetDeviceRemovedReason(const ComPtr<ID3D12Device>&, char* message);
}