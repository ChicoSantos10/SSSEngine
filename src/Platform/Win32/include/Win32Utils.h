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
namespace SSSWin32
{
    using namespace Microsoft::WRL;

    void ThrowIfFailed(HRESULT hr);
    void GetErrorMessage(HRESULT hr, char *message);
    // void GetDeviceRemovedReason(const ComPtr<ID3D12Device>&, char* message);
} // namespace SSSWin32
