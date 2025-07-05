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
#include "Constants.h"
#include "Window.h"
#include "d3d12.h"
#include "Types.h"
#include <wrl/client.h>
#include <dxgi1_6.h>

namespace SSSRenderer::SSSDirectx12
{

    // INVESTIGATE: Is this a proper name?
    /**
     * @class RenderingContext
     * @brief Represents a visual part of a window
     * This could be a virtual window inside of an actual window or the window itself
     *
     */
    class RenderingContext
    {
        template<typename T>
        using ComPtr = Microsoft::WRL::ComPtr<T>;

        public:
        explicit RenderingContext(const SSSEngine::Window &window);
        RenderingContext(RenderingContext &&) = default; // INVESTIGATE: Do we need to define 1?
        RenderingContext &operator=(const RenderingContext &) = delete;
        RenderingContext &operator=(RenderingContext &&) = delete;
        RenderingContext(const RenderingContext &context) = delete;
        virtual ~RenderingContext();

        void ResizeSwapChain(u32 width, u32 height);
        void CreateRtv();
        void CreateDepthStencilBuffer(u32 width, u32 height);
        void Flush();
        void Signal();
        void WaitForFenceValue();
        void
        Render(const ComPtr<ID3D12PipelineState> &, const ComPtr<ID3D12RootSignature> &, const D3D12_VERTEX_BUFFER_VIEW &);

        // Descriptors
        ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
        ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
        UINT rtvDescriptorSize = 0;
        UINT dsvDescriptorSize = 0;
        UINT cbvSrvUavDescriptorSize = 0;

        ComPtr<IDXGISwapChain4> swapChain;
        ComPtr<ID3D12Resource> backBuffers[BackBuffersAmount];
        ComPtr<ID3D12Resource> depthStencilBuffer;
        ComPtr<ID3D12CommandAllocator> commandAllocators[BackBuffersAmount];
        ComPtr<ID3D12CommandQueue> commandQueue;
        ComPtr<ID3D12GraphicsCommandList> commandList;

        // Fence
        ComPtr<ID3D12Fence> fence;
        u64 frameFenceValues[BackBuffersAmount] = {0};
        HANDLE fenceEvent = nullptr;

        // Refresh Rate and Variable Refresh Rate support
        BOOL allowTearing = false;

        // Viewport
        D3D12_VIEWPORT viewport{};
        D3D12_RECT scissorRect{};

        // Constants
        // INVESTIGATE: Where should this be. Are they useful as constants
        static constexpr DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        static constexpr DXGI_FORMAT ClearValueFormat = DXGI_FORMAT_D32_FLOAT;
    };

    // TODO: This should be in a file of it's own since it's just a helper function
    SSSENGINE_FORCE_INLINE D3D12_CPU_DESCRIPTOR_HANDLE
    GetDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> &descriptorHeap)
    {
#if defined(_MSC_VER) || !defined(_WIN32)
        return descriptorHeap->GetCPUDescriptorHandleForHeapStart();
#else
        D3D12_CPU_DESCRIPTOR_HANDLE handle;
        descriptorHeap->GetCPUDescriptorHandleForHeapStart(&handle);
        return handle;
#endif
    }
} // namespace SSSRenderer::SSSDirectx12
