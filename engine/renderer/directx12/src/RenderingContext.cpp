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

/**
 * @file
 * @brief
 */

#include "RenderingContext.h"
#include "Constants.h"
#include "Device.h"
#include "Factory.h"
#include "Platform.h"
#include "d3d12.h"
#include "d3dx12_barriers.h"
#include "d3dx12_core.h"
#include "d3dx12_root_signature.h"
#include "Win32Utils.h"

namespace SSSEngine::Renderer::DirectX12
{
    RenderingContext::RenderingContext(const SSSEngine::Platform::WindowHandle &window)
    {
        // Command Queue
        {
            D3D12_COMMAND_QUEUE_DESC desc;
            desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
            desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            desc.NodeMask = 0;

            SSSENGINE_THROW_IF_FAILED(Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue)));
        }

        // Create Command Allocators
        // TODO: We need a Command Allocator for each command list + for each thread (backBuffers * threads)
        {
            for(auto &commandAllocator: commandAllocators)
            {
                SSSENGINE_THROW_IF_FAILED(
                    Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
            }
        }

        // Create Command List
        // TODO: We need a command list for each thread
        {
            // INVESTIGATE: Should we define the initial pipeline state here?
            SSSENGINE_THROW_IF_FAILED(Device->CreateCommandList(
                0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocators[0].Get(), nullptr, IID_PPV_ARGS(&commandList)));

            SSSENGINE_THROW_IF_FAILED(commandList->Close());
        }

        // Descriptor Sizes
        {
            rtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
            dsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
            cbvSrvUavDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        }

        // Create RTV Descriptor Heap
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc = {};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.NumDescriptors = BackBuffersAmount;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            desc.NodeMask = 0;

            SSSENGINE_THROW_IF_FAILED(Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&rtvDescriptorHeap)));
        }

        // Create DSV Descriptor Heap
        {
            D3D12_DESCRIPTOR_HEAP_DESC dsvDesc{};
            dsvDesc.NumDescriptors = 1;
            dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            SSSENGINE_THROW_IF_FAILED(Device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(&dsvDescriptorHeap)));
        }

        // Create Fence
        {
            SSSENGINE_THROW_IF_FAILED(Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
        }
        // Swap Chain
        {
            // INVESTIGATE: Do we need to reset?
            // swapChain.Reset();

            SSSENGINE_THROW_IF_FAILED(
                Factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing)));

            const auto handle = static_cast<HWND>(window);

            DXGI_SWAP_CHAIN_DESC1 desc = {};
            desc.Width = 0; // NOTE: Passing 0 means use window height and width
            desc.Height = 0;
            desc.Format = BackBufferFormat;
            desc.Stereo = false;
            desc.SampleDesc.Count = 1;
            desc.SampleDesc.Quality = 0;
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.BufferCount = BackBuffersAmount;
            desc.Scaling = DXGI_SCALING_STRETCH;
            desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            UINT flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
            if(allowTearing)
                flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
            desc.Flags = flags;

            Microsoft::WRL::ComPtr<IDXGISwapChain1> chain;
            SSSENGINE_THROW_IF_FAILED(
                Factory->CreateSwapChainForHwnd(commandQueue.Get(), handle, &desc, nullptr, nullptr, &chain));
            SSSENGINE_THROW_IF_FAILED(chain.As(&swapChain));

            SSSENGINE_THROW_IF_FAILED(Factory->MakeWindowAssociation(handle, DXGI_MWA_NO_ALT_ENTER));

            RECT rect;
            const BOOL success = GetWindowRect(handle, &rect);
            SSSENGINE_ASSERT(success); // INVESTIGATE: Should we throw if failed in release versions?
            const LONG width = rect.right - rect.left;
            const LONG height = rect.bottom - rect.top;
            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            viewport.Width = static_cast<FLOAT>(width);
            viewport.Height = static_cast<FLOAT>(height);

            scissorRect.top = 0;
            scissorRect.left = 0;
            scissorRect.right = width;
            scissorRect.bottom = height;

            float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
            DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, aspectRatio, 1, 1000);
            DirectX::XMStoreFloat4x4(&projectionMatrix, projection);

            CreateDepthStencilBuffer(width, height);
            CreateRtv();

            // TODO: Link the window to the RenderingContext
        }
    }

    void RenderingContext::CreateRtv()
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(GetDescriptorHandle(rtvDescriptorHeap));

        for(UINT i = 0; i < BackBuffersAmount; ++i)
        {
            SSSENGINE_THROW_IF_FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i])));

            Device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, rtvDescriptorSize);
        }
    }

    void RenderingContext::Flush()
    {
        Signal();
        WaitForFenceValue();
    }

    void RenderingContext::Signal()
    {
        const auto index = swapChain->GetCurrentBackBufferIndex();
        SSSENGINE_THROW_IF_FAILED(commandQueue->Signal(fence.Get(), ++frameFenceValues[index]));
    }

    void RenderingContext::WaitForFenceValue()
    {
        const auto index = swapChain->GetCurrentBackBufferIndex();
        if(const auto value = frameFenceValues[index]; fence->GetCompletedValue() < value)
        {
            SSSENGINE_THROW_IF_FAILED(fence->SetEventOnCompletion(value, fenceEvent));
            WaitForSingleObject(fenceEvent, INFINITE);
        }
    }

    void RenderingContext::CreateDepthStencilBuffer(const uint32_t width, const uint32_t height)
    {
        SSSENGINE_ASSERT(width > 0 && height > 0);

        Flush();

        D3D12_CLEAR_VALUE clearValue;
        clearValue.Format = DepthStencilFormat;
        clearValue.DepthStencil = {.Depth = 1.0f, .Stencil = 0};

        // TODO: This function should know the current sample count and quality
        const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
        const auto resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            DepthStencilFormat, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

        SSSENGINE_THROW_IF_FAILED(Device->CreateCommittedResource(&heapProperties,
                                                                  D3D12_HEAP_FLAG_NONE,
                                                                  &resourceDesc,
                                                                  D3D12_RESOURCE_STATE_DEPTH_WRITE,
                                                                  &clearValue,
                                                                  IID_PPV_ARGS(depthStencilBuffer.ReleaseAndGetAddressOf())));

        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc;
        depthStencilDesc.Format = DepthStencilFormat;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
        depthStencilDesc.Texture2D.MipSlice = 0;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

        Device->CreateDepthStencilView(
            depthStencilBuffer.Get(), &depthStencilDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    }

    void RenderingContext::Render(const Microsoft::WRL::ComPtr<ID3D12PipelineState> &pipelineState,
                                  const Microsoft::WRL::ComPtr<ID3D12RootSignature> &rootSignature,
                                  const D3D12_VERTEX_BUFFER_VIEW &vertexBufferView,
                                  const D3D12_INDEX_BUFFER_VIEW &indexBufferView,
                                  const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> &descriptorHeap)
    {
        // Populate command list
        {
            const auto backBufferIndex = swapChain->GetCurrentBackBufferIndex();
            const auto commandAllocator = commandAllocators[backBufferIndex];
            const auto backBuffer = backBuffers[backBufferIndex];

            commandList->SetPipelineState(pipelineState.Get());
            commandList->SetGraphicsRootSignature(rootSignature.Get());

            ID3D12DescriptorHeap *descriptorHeaps[] = {descriptorHeap.Get()};
            commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
            commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
            CD3DX12_GPU_DESCRIPTOR_HANDLE cbv(descriptorHeap->GetGPUDescriptorHandleForHeapStart());
            commandList->SetGraphicsRootDescriptorTable(0, cbv);

            commandList->RSSetViewports(1, &viewport);
            commandList->RSSetScissorRects(1, &scissorRect);

            auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
            commandList->ResourceBarrier(1, &barrier);

            const CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
                GetDescriptorHandle(rtvDescriptorHeap), static_cast<int>(backBufferIndex), rtvDescriptorSize);
            const CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(GetDescriptorHandle(dsvDescriptorHeap));
            commandList->OMSetRenderTargets(1, &rtvHandle, true, &dsvHandle);

            constexpr float ClearColor[]{0.5f, 0.5f, 0.75f, 1.0f};
            commandList->ClearRenderTargetView(rtvHandle, ClearColor, 0, nullptr);
            commandList->ClearDepthStencilView(dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
                                               D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
                                               1.0f,
                                               0,
                                               0,
                                               nullptr);
            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
            commandList->IASetIndexBuffer(&indexBufferView);
            commandList->DrawIndexedInstanced(36, 1, 0, 0, 0);

            barrier = CD3DX12_RESOURCE_BARRIER::Transition(
                backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            commandList->ResourceBarrier(1, &barrier);

            SSSENGINE_THROW_IF_FAILED(commandList->Close());
        }

        ID3D12CommandList *commandLists[] = {commandList.Get()};
        commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

        // INVESTIGATE:
        //  - Should we use the Present1 function instead?
        //  - Best Sync Interval
        // TODO: Also check for windowed mode
        if(allowTearing)
            SSSENGINE_THROW_IF_FAILED(swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));
        else
            SSSENGINE_THROW_IF_FAILED(swapChain->Present(1, 0));

        Flush();
    }

    void RenderingContext::ResizeSwapChain(u32 width, u32 height)
    {
        DXGI_SWAP_CHAIN_DESC1 desc;
        // INVESTIGATE: Should we throw
        SSSENGINE_THROW_IF_FAILED(swapChain->GetDesc1(&desc));
        if(desc.Width == static_cast<UINT>(width) && desc.Height == static_cast<UINT>(height))
            return;

        viewport.Width = static_cast<FLOAT>(width);
        viewport.Height = static_cast<FLOAT>(height);
        scissorRect.bottom = static_cast<LONG>(height);
        scissorRect.right = static_cast<LONG>(width);

        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        DirectX::XMMATRIX projection = DirectX::XMMatrixPerspectiveFovLH(0.25f * DirectX::XM_PI, aspectRatio, 1, 1000);
        DirectX::XMStoreFloat4x4(&projectionMatrix, projection);

        Flush();

        const auto backBufferIndex = swapChain->GetCurrentBackBufferIndex();
        for(int i = 0; i < BackBuffersAmount; ++i)
        {
            backBuffers[i].Reset();
            frameFenceValues[i] = frameFenceValues[backBufferIndex];
        }

        SSSENGINE_THROW_IF_FAILED(swapChain->ResizeBuffers(BackBuffersAmount, width, height, desc.Format, desc.Flags));

        // INVESTIGATE: Should this be it's own function? Since we usually call this 2 functions together
        CreateDepthStencilBuffer(width, height);
        CreateRtv();
    }

    void RenderingContext::BeginFrame()
    {
        const auto backBufferIndex = swapChain->GetCurrentBackBufferIndex();
        const auto commandAllocator = commandAllocators[backBufferIndex];
        const auto backBuffer = backBuffers[backBufferIndex];
        SSSENGINE_THROW_IF_FAILED(commandAllocator->Reset());
        SSSENGINE_THROW_IF_FAILED(commandList->Reset(commandAllocator.Get(), nullptr));
    }

    void RenderingContext::Terminate()
    {
        Flush();
        CloseHandle(fenceEvent);
        swapChain.Reset();
        for(int i = 0; i < BackBuffersAmount; ++i)
        {
            backBuffers[i].Reset();
            commandAllocators[i].Reset();
        }
        depthStencilBuffer.Reset();
        commandQueue.Reset();
        commandList.Reset();
        fence.Reset();
    }
} // namespace SSSEngine::Renderer::DirectX12
