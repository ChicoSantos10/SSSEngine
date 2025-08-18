/*  SSS Engine
    Copyright (C) 2025  Francisco Santos

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
    USA
*/

/**
 * @file
 * @brief
 */

#pragma once

#include <windows.h>
#include <wrl/client.h>
#include "Debug.h"
// #include "directx/d3d12.h"

// INVESTIGATE: If we just assert we lose the error message. Figure out a way to keep it
#ifdef SSSENGINE_DEBUG
    #define SSSENGINE_THROW_IF_FAILED(hr) SSSENGINE_ASSERT(SUCCEEDED(hr))
#else
    #define SSSENGINE_THROW_IF_FAILED(hr) ThrowIfFailed(hr)
#endif
namespace SSSEngine::Platform::Win32
{
    void ThrowIfFailed(HRESULT hr);
    void GetErrorMessage(HRESULT hr, char *message);
    // void GetDeviceRemovedReason(const ComPtr<ID3D12Device>&, char* message);
} // namespace SSSEngine::Platform::Win32
