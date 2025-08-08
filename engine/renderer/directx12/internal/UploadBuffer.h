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

#include "Attributes.h"
#include "Device.h"
#include "Types.h"
#include "Win32Utils.h"
#include "d3d12.h"
#include "d3dx12_core.h"
#include <cstring>

namespace SSSEngine::Renderer::DirectX12
{
    consteval u32 GetConstantBufferByteSize(u64 size) { return (size + 255) & ~255; }

    template<typename T, bool IsConstantBuffer>
    class UploadBuffer
    {
        public:
        explicit UploadBuffer(u32 count)
        {
            auto heaptype = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(Size * count);
            Platform::Win32::ThrowIfFailed(Device->CreateCommittedResource(
                &heaptype, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_uploadBuffer)));

            Platform::Win32::ThrowIfFailed(m_uploadBuffer->Map(0, nullptr, reinterpret_cast<void **>(&m_data)));
        }
        UploadBuffer(const UploadBuffer &rhs) = delete;
        UploadBuffer(UploadBuffer &&) = delete;
        UploadBuffer &operator=(const UploadBuffer &rhs) = delete;
        UploadBuffer &operator=(UploadBuffer &&) = delete;

        ~UploadBuffer()
        {
            m_uploadBuffer->Unmap(0, nullptr);
            m_data = nullptr;
        }

        SSSENGINE_FORCE_INLINE void CopyData(u32 index, const T &data)
        {
            memcpy(&m_data[index * Size], &data, sizeof(T));
        }

        static constexpr u64 Size = IsConstantBuffer ? GetConstantBufferByteSize(sizeof(T)) : sizeof(T);

        SSSENGINE_PURE Microsoft::WRL::ComPtr<ID3D12Resource> GetBufferResource() const noexcept
        {
            return m_uploadBuffer;
        }

        private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer;
        byte *m_data{};
    };
} // namespace SSSEngine::Renderer::DirectX12
