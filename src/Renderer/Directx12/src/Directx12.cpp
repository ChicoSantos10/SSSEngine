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

// TODO: Remove std library
#include <iostream>
#include <memory>
#include <vector>

#include "initguid.h"
#include "d3dx12_core.h"
#include <windows.h>
#include "Types.h"
#include "d3dcommon.h"
#include "d3dx12_root_signature.h"
#include "dxgiformat.h"
#include "d3d12.h"
#include "d3dx12.h"
#include "DirectXMath.h"
#include "Factory.h"
#include "Win32Utils.h"
#include "dxcapi.h"
#include "dxgi1_6.h"

#include "Device.h"
#include "RenderingContext.h"
#include "Vertex.h"
#include "DefaultBuffer.h"
#include "../../../Platform/Common/include/Window.h"
#include "Attributes.h"
#include "Constants.h"
#include "Debug.h"
#include "UploadBuffer.h"

// TODO: LOG function/Macro for HR results

namespace SSSRenderer::SSSDirectx12
{
    // TODO: Set default values
    namespace
    {
        using namespace SSSWin32;

        // TODO: Replace vector with custom array like structure
        std::vector<RenderingContext> RenderingContexts;

        ComPtr<ID3D12RootSignature> RootSignature;
        ComPtr<ID3D12PipelineState> PipelineState;
        ComPtr<ID3D12InfoQueue> InfoQueue;

        ComPtr<ID3D12Resource> IntermediateBuffer; // NOTE: This buffer needs to exist until its contents are copied to
                                                   // the vertex buffer
        ComPtr<ID3D12Resource> VertexBuffer;
        D3D12_VERTEX_BUFFER_VIEW VertexBufferView;
        ComPtr<ID3D12Resource> IndexBuffer;
        D3D12_INDEX_BUFFER_VIEW IndexBufferView;

        // Constant buffer
        std::unique_ptr<UploadBuffer<DirectX::XMFLOAT4X4, true>> ObjectMatrixBuffer;
        ComPtr<ID3D12DescriptorHeap> ObjectMatrixBufferDescriptor;

        // AA
        UINT MsaaMaxQualityLevelsSupported = 0;

    } // namespace

    // LOW_PRIORITY: Put this in a separate file
    D3D_FEATURE_LEVEL FindMaxFeatureLevel()
    {
        // TODO: Check if this is correct
        constexpr D3D_FEATURE_LEVEL Features[] = {
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_12_0,
            D3D_FEATURE_LEVEL_12_1,
            D3D_FEATURE_LEVEL_12_2,
        };

        D3D12_FEATURE_DATA_FEATURE_LEVELS levels{_countof(Features), Features, Features[0]};

        Device->CheckFeatureSupport(D3D12_FEATURE_FEATURE_LEVELS, &levels, sizeof(levels));

        return levels.MaxSupportedFeatureLevel;
    }

    SSSENGINE_DLL_EXPORT void Initialize()
    {
        using namespace SSSWin32;

#ifdef SSSENGINE_DEBUG_GRAPHICS
        // Enabling debug
        {
            ComPtr<ID3D12Debug> debugInterface;
            if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface))))
            {
                SSSENGINE_LOG_INFO("Enabling Debug Interface");
                debugInterface->EnableDebugLayer();
            }

            ComPtr<ID3D12Debug1> debug1;
            if(SUCCEEDED(debugInterface.As(&debug1)))
            {
                SSSENGINE_LOG_INFO("Enabling Debug Interface 1");
                debug1->SetEnableGPUBasedValidation(true);
            }
        }
#endif

        // Creating Device
        {
            ComPtr<IDXGIFactory2> factory2;
            UINT factoryFlags = 0;

#ifdef SSSENGINE_DEBUG_GRAPHICS
            factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

            SSSENGINE_THROW_IF_FAILED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory2)));
            SSSENGINE_THROW_IF_FAILED(factory2.As(&Factory));

            ComPtr<IDXGIAdapter4> adapter;
            SSSENGINE_THROW_IF_FAILED(
                Factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)));

            SSSENGINE_THROW_IF_FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&Device)));
        }

#ifdef SSSENGINE_DEBUG_GRAPHICS
        // Create Info Queue
        {
            SSSENGINE_THROW_IF_FAILED(Device->QueryInterface(IID_PPV_ARGS(&InfoQueue)));
            InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
            InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
            InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

            D3D12_MESSAGE_SEVERITY severities[] = {
                D3D12_MESSAGE_SEVERITY_INFO,
                D3D12_MESSAGE_SEVERITY_WARNING,
                D3D12_MESSAGE_SEVERITY_ERROR,
                D3D12_MESSAGE_SEVERITY_CORRUPTION,
                D3D12_MESSAGE_SEVERITY_MESSAGE,
            };

            D3D12_MESSAGE_ID denyIds[] = {D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
                                          D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
                                          D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE};

            D3D12_INFO_QUEUE_FILTER newFilter = {};
            newFilter.AllowList.NumSeverities = _countof(severities);
            newFilter.AllowList.pSeverityList = severities;
            newFilter.DenyList.NumSeverities = 0;
            newFilter.DenyList.pSeverityList = nullptr;
            newFilter.DenyList.NumIDs = _countof(denyIds);
            newFilter.DenyList.pIDList = denyIds;

            InfoQueue->PushStorageFilter(&newFilter);
        }
#endif

        // Root signature
        {
            // Create a root signature.
            D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{D3D_ROOT_SIGNATURE_VERSION_1_1};
            if(FAILED(Device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
            {
                // TODO: Maybe just log or SSSENGINE_ASSERT that it passes
                featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
            }

            // Allow input layout and deny unnecessary access to certain pipeline stages.
            constexpr D3D12_ROOT_SIGNATURE_FLAGS RootSignatureFlags =
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

            CD3DX12_ROOT_PARAMETER rootParameters[1];
            // rootParameters[0].InitAsConstants(
            //     static_cast<UINT>(sizeof(DirectX::XMMATRIX) * 0.25f), 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

            CD3DX12_DESCRIPTOR_RANGE cbvTable;
            cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

            rootParameters[0].InitAsDescriptorTable(1, &cbvTable);

            CD3DX12_ROOT_SIGNATURE_DESC rootSig(1, rootParameters, 0, nullptr, RootSignatureFlags);

            ComPtr<ID3DBlob> signature;
            ComPtr<ID3DBlob> error;

            // TODO: Use error and figure out why the version needs to be 1 here
            SSSENGINE_THROW_IF_FAILED(D3D12SerializeRootSignature(&rootSig, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
            SSSENGINE_THROW_IF_FAILED(Device->CreateRootSignature(
                0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&RootSignature)));
        }

        // Anti Aliasing Support
        // TODO: Check all supported options
        // TODO: Implement Antialiasing in the render pipeline
        {
            // 4x MSAA
            D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS qualityLevels{
                .Format = RenderingContext::BackBufferFormat,
                .SampleCount = 4,
                .Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE,
                .NumQualityLevels = 0,
            };

            SSSENGINE_THROW_IF_FAILED(Device->CheckFeatureSupport(
                D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &qualityLevels, sizeof(qualityLevels)));

            MsaaMaxQualityLevelsSupported = qualityLevels.NumQualityLevels;
            SSSENGINE_ASSERT(MsaaMaxQualityLevelsSupported > 0);
        }

        // PSO
        {
            constexpr LPCWSTR FilePath = LR"(N:\C++Projects\SSSEngine\src\Renderer\Shaders\TestShader.hlsl)";

            ComPtr<IDxcUtils> compilerUtils;
            ComPtr<IDxcCompiler3> shaderCompiler;
            DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&compilerUtils));
            DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&shaderCompiler));

            ComPtr<IDxcIncludeHandler> includeHandler;
            compilerUtils->CreateDefaultIncludeHandler(&includeHandler);

            LPCWSTR commandArgsVs[] = {
                L"TestShader.hlsl",
                // Optional shader src file
                L"-E",
                L"vertex",
                L"-T",
                L"vs_6_6",
                L"-Zs",
                L"-Qstrip_reflect",
                L"-Fo",
                L"TestShader.bin",
                L"-Fd",
                L"TestShader.pdb",
            };

            LPCWSTR commandArgsPs[] = {
                L"TestShader.hlsl",
                // Optional shader src file
                L"-E",
                L"fragment",
                L"-T",
                L"ps_6_6",
                L"-Zs",
                L"-Qstrip_reflect",
                L"-Fo",
                L"TestShader.bin",
                L"-Fd",
                L"TestShader.pdb",
            };

            ComPtr<IDxcBlobEncoding> encoder;
            compilerUtils->LoadFile(FilePath, nullptr, &encoder);

            DxcBuffer buffer{};
            buffer.Ptr = encoder->GetBufferPointer();
            buffer.Size = encoder->GetBufferSize();
            buffer.Encoding = DXC_CP_ACP;

            auto debug = [](const ComPtr<IDxcResult> &result)
            {
                ComPtr<IDxcBlobUtf8> errors;
                result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr);
                if(errors && errors->GetStringLength() > 0)
                {
                    std::cout << errors->GetStringPointer() << std::endl;
                }

                HRESULT hr;
                result->GetStatus(&hr);
                if(FAILED(hr))
                {
                    std::cout << "Failed vertex shader compilation" << std::endl;
                }
            };

            ComPtr<IDxcResult> vsResult;
            ComPtr<IDxcResult> psResult;

            shaderCompiler->Compile(
                &buffer, commandArgsVs, _countof(commandArgsVs), includeHandler.Get(), IID_PPV_ARGS(&vsResult));
            shaderCompiler->Compile(
                &buffer, commandArgsPs, _countof(commandArgsPs), includeHandler.Get(), IID_PPV_ARGS(&psResult));

            debug(vsResult);
            debug(psResult);

            ComPtr<ID3DBlob> vertexShader;
            ComPtr<ID3DBlob> fragmentShader;

            vsResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&vertexShader), nullptr);
            psResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&fragmentShader), nullptr);

#ifdef SSSENGINE_FXC
            UINT compilerFlags = 0;
            ComPtr<ID3DBlob> errorMsgs;

    #ifdef SSSENGINE_DEBUG_GRAPHICS
            compilerFlags |= (D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION);
    #endif

            auto DebugCompilation = [&](HRESULT hr)
            {
                if(FAILED(hr))
                {
                    auto msg = reinterpret_cast<const char *>(errorMsgs->GetBufferPointer());
                    std::cout << msg << std::endl;
                }
            };

            DebugCompilation(D3DCompileFromFile(
                filePath, nullptr, nullptr, "vertex", "vs_5_0", compilerFlags, 0, &vertexShader, &errorMsgs));
            DebugCompilation(D3DCompileFromFile(
                filePath, nullptr, nullptr, "fragment", "ps_5_0", compilerFlags, 0, &fragmentShader, &errorMsgs));
#endif
            constexpr u64 NormalOffset = sizeof(Vertex::Position);
            constexpr u64 ColorOffset = NormalOffset + sizeof(Vertex::Normal);

            D3D12_INPUT_ELEMENT_DESC inputDesc[]{
                {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                {"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, NormalOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
                {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, ColorOffset, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
            };

            // TODO: MSAA 4x
            D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
            ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
            psoDesc.InputLayout = {inputDesc, _countof(inputDesc)};
            psoDesc.pRootSignature = RootSignature.Get();
            psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
            psoDesc.PS = CD3DX12_SHADER_BYTECODE(fragmentShader.Get());
            psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
            psoDesc.DepthStencilState.DepthEnable = false;
            psoDesc.DepthStencilState.StencilEnable = false;
            psoDesc.SampleMask = UINT_MAX;
            psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            psoDesc.NumRenderTargets = 1;
            psoDesc.RTVFormats[0] = RenderingContext::BackBufferFormat;
            psoDesc.SampleDesc.Count = 1;
            psoDesc.SampleDesc.Quality = 0;
            psoDesc.DSVFormat = RenderingContext::DepthStencilFormat;

            SSSENGINE_THROW_IF_FAILED(Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PipelineState)));
        }

        // TODO: Remove this
        {
            ObjectMatrixBuffer = std::make_unique<UploadBuffer<DirectX::XMFLOAT4X4, true>>(1);

            D3D12_DESCRIPTOR_HEAP_DESC cbvHeadDesc;
            cbvHeadDesc.NumDescriptors = 1;
            cbvHeadDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
            cbvHeadDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            cbvHeadDesc.NodeMask = 0;
            Device->CreateDescriptorHeap(&cbvHeadDesc, IID_PPV_ARGS(&ObjectMatrixBufferDescriptor));

            u32 objByteSize = UploadBuffer<DirectX::XMFLOAT4X4, true>::Size;

            D3D12_GPU_VIRTUAL_ADDRESS cbAddress = ObjectMatrixBuffer->GetBufferResource()->GetGPUVirtualAddress();

            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
            cbvDesc.BufferLocation = cbAddress;
            cbvDesc.SizeInBytes = objByteSize;

            Device->CreateConstantBufferView(&cbvDesc, ObjectMatrixBufferDescriptor->GetCPUDescriptorHandleForHeapStart());
        }
    }

    SSSENGINE_DLL_EXPORT void Render()
    {
        for(auto &renderingContext: RenderingContexts)
        {
            using namespace DirectX;

            XMVECTOR pos = XMVectorSet(0, 0, -10, 1);
            XMVECTOR target = XMVectorZero();
            XMVECTOR up = XMVectorSet(0, 1, 0, 0);
            XMMATRIX view = XMMatrixLookAtLH(pos, target, up);

            XMMATRIX world = XMMatrixIdentity();
            XMMATRIX proj = XMMatrixPerspectiveFovLH(0.25f * XM_PI, 1920.0f / 1080, 1, 1000);

            XMMATRIX worldViewProj = world * view * proj;

            XMFLOAT4X4 objectWVP{};
            XMStoreFloat4x4(&objectWVP, XMMatrixTranspose(worldViewProj));

            ObjectMatrixBuffer->CopyData(0, objectWVP);

            renderingContext.Render(
                PipelineState, RootSignature, VertexBufferView, IndexBufferView, ObjectMatrixBufferDescriptor);
        }
    }

    SSSENGINE_DLL_EXPORT void BeginFrame()
    {
        for(auto &renderingContext: RenderingContexts)
        {
            renderingContext.BeginFrame();
        }
    }

    // TODO: Use the swap chain from window
    SSSENGINE_DLL_EXPORT void ResizeSwapChain(const SSSEngine::Window &window)
    {
        const auto handle = static_cast<HWND>(window.GetHandle());
        RECT rect;
        GetWindowRect(handle, &rect);

        const auto width = rect.right - rect.left;
        const auto height = rect.bottom - rect.top;

        SSSENGINE_ASSERT(width > 0 && height > 0 && "Must pass appropriate values for width and height");

        // TODO: Use the window as the index to find the correct swap chain to resize
        RenderingContexts[0].ResizeSwapChain(width, height);
    }

    SSSENGINE_DLL_EXPORT void CreateSwapChain(const SSSEngine::Window &window)
    {
        RenderingContexts.emplace_back(window);
    }

    // TODO: Remove this eventually
    SSSENGINE_DLL_EXPORT void LoadAssetsTest()
    {
        // Vertex Buffer
        {
            constexpr Vertex Vertices[]{{.Position = {0.0f, 0.5f, 0.0f}, .Color = {{1.0f, 0.0f, 0.0f}, 1.0f}},
                                        {.Position = {0.25f, -0.5f, 0.0f}, .Color = {{0.0f, 1.0f, 0.0f}, 1.0f}},
                                        {.Position = {-0.25f, -0.5f, 0.0f}, .Color = {{0.0f, 0.0f, 1.0f}, 1.0f}}};

            constexpr UINT VertexBufferSize = sizeof(Vertices);

            constexpr u16 Indices[]{0, 1, 2};
            constexpr UINT IndexBufferSize = sizeof(Indices);

            SSSENGINE_ASSERT(RenderingContexts.size() > 0);
            SSSENGINE_ASSERT(RenderingContexts[0].commandList.Get());

            auto cmdList = RenderingContexts[0].commandList.Get();
            VertexBuffer = CreateDefaultBuffer(cmdList, Vertices, VertexBufferSize, IntermediateBuffer);

            IndexBuffer = CreateDefaultBuffer(cmdList, Indices, IndexBufferSize, IntermediateBuffer);

            // This was copying data directly into the vertex buffer
            // UINT8 *begin = nullptr;
            // const CD3DX12_RANGE readRange(0, 0);
            // SSSENGINE_THROW_IF_FAILED(VertexBuffer->Map(0, &readRange, reinterpret_cast<void **>(&begin)));
            // memcpy(begin, Vertices, VertexBufferSize);

            // VertexBuffer->Unmap(0, nullptr);
            VertexBufferView.BufferLocation = VertexBuffer->GetGPUVirtualAddress();
            VertexBufferView.StrideInBytes = sizeof(Vertex);
            VertexBufferView.SizeInBytes = VertexBufferSize;

            IndexBufferView.BufferLocation = IndexBuffer->GetGPUVirtualAddress();
            IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
            IndexBufferView.SizeInBytes = IndexBufferSize;
        }

        // NOTE: No need to flush since this is loaded before we even commit drawing commands
        //		In the future we need to do this when commiting resources to the gpu
        // Flush();
    }

    SSSENGINE_DLL_EXPORT void Terminate()
    {
        // INVESTIGATE: For now it's being done in the destructor of renderingContext but where should flushing and
        // CloseHandle be?

        /*Flush();*/
        /*CloseHandle(fenceEvent);*/
    }
} // namespace SSSRenderer::SSSDirectx12
