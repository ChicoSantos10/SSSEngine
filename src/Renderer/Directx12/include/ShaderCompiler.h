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

#include "d3dcommon.h"
#include "wrl/client.h"
#include "dxcapi.h"

#include "Logger.h"
#include <tuple>

namespace SSSRenderer::SSSDirectx12
{
    // TODO: Maybe should have this in a windowspointer.h file
    template<typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    struct Shader
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> fragmentShader;
    };

    Shader CompileShader(LPCWSTR file)
    {
        ComPtr<IDxcUtils> compilerUtils;
        ComPtr<IDxcCompiler3> shaderCompiler;
        DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&compilerUtils));
        DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&shaderCompiler));

        ComPtr<IDxcIncludeHandler> includeHandler;
        compilerUtils->CreateDefaultIncludeHandler(&includeHandler);

        LPCWSTR commandArgsVs[] = {
            file,
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
            file,
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
        compilerUtils->LoadFile(file, nullptr, &encoder);

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
                SSSENGINE_LOG_ERROR("{}", LPWSTR(errors->GetStringPointer()));
            }

            HRESULT hr;
            result->GetStatus(&hr);
            if(FAILED(hr))
            {
                SSSENGINE_LOG_ERROR("Failed Shader Compilation");
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

        return {.vertexShader = vertexShader, .fragmentShader = fragmentShader};

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
    }
} // namespace SSSRenderer::SSSDirectx12
