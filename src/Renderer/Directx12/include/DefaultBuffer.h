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

// INVESTIGATE: File name

#include "Debug.h"
#include "Device.h"
#include "Types.h"
#include "Win32Utils.h"
#include "d3d12.h"
#include "d3dx12_barriers.h"
#include "d3dx12_core.h"
#include "d3dx12_resource_helpers.h"
#include "wrl/client.h"

namespace SSSRenderer::SSSDirectx12
{
    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    ComPtr<ID3D12Resource> CreateDefaultBuffer(ID3D12GraphicsCommandList *cmdList, const void *data, u64 byteSize,
                                               ComPtr<ID3D12Resource> &uploadBuffer)
    {
        SSSENGINE_ASSERT(cmdList != nullptr);
        SSSENGINE_ASSERT(data);

        ComPtr<ID3D12Resource> defaultBuffer;
        {
            auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            auto buffer = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
            SSSWin32::ThrowIfFailed(Device->CreateCommittedResource(&heapType,
                                                                    D3D12_HEAP_FLAG_NONE,
                                                                    &buffer,
                                                                    D3D12_RESOURCE_STATE_COMMON,
                                                                    nullptr,
                                                                    IID_PPV_ARGS(defaultBuffer.GetAddressOf())));
        }
        // Intermediate Heap to upload data since we cannot upload directly to a default heap
        {
            auto heapType = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto buffer = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
            SSSWin32::ThrowIfFailed(Device->CreateCommittedResource(&heapType,
                                                                    D3D12_HEAP_FLAG_NONE,
                                                                    &buffer,
                                                                    D3D12_RESOURCE_STATE_GENERIC_READ,
                                                                    nullptr,
                                                                    IID_PPV_ARGS(uploadBuffer.GetAddressOf())));
        }

        D3D12_SUBRESOURCE_DATA subResourceData{
            .pData = data,
            .RowPitch = static_cast<LONG_PTR>(byteSize),
            .SlicePitch = static_cast<LONG_PTR>(byteSize),
        };

        {
            auto transition = CD3DX12_RESOURCE_BARRIER::Transition(
                defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
            cmdList->ResourceBarrier(1, &transition);
        }

        UpdateSubresources(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

        {
            auto transition = CD3DX12_RESOURCE_BARRIER::Transition(
                defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
            cmdList->ResourceBarrier(1, &transition);
        }

        return defaultBuffer;
    }
} // namespace SSSRenderer::SSSDirectx12
