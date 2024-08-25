//
// Created by Francisco Santos on 11/07/2023.
//

#pragma once

#include <windows.h>
#include <wrl/client.h>
//#include "directx/d3d12.h"

// TODO: Fix release version and also improve the debug version as well
#ifdef SSSENGINE_DEBUG
#define SSSENGINE_THROW_IF_FAILED(hr)  if(!FAILED(hr)){} else 				\
	{																\
		SSSENGINE_DEBUG_BREAK;\
	}
#else
#define SSSENGINE_THROW_IF_FAILED(hr) if (FAILED(hr))				\
	{\
		char message[1024];											\
		GetErrorMessage(hr, message);								\
		std::cout << message << std::endl;							\
																	\
		throw std::exception();										\
	}
#endif
namespace SSSWin32
{
	using namespace Microsoft::WRL;

	void ThrowIfFailed(HRESULT hr);
	void GetErrorMessage(HRESULT hr, char *message);
	//void GetDeviceRemovedReason(const ComPtr<ID3D12Device>&, char* message);
}
