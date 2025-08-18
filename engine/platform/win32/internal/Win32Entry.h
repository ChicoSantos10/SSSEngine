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

#include <cstdio>
#include <windows.h>
#include <wrl/client.h>
#include <xinput.h>
#include "Platform.h"
#include "Win32Window.h"

// Timing

// Debugs
#include <dbghelp.h>
// #include <dia2.h>
#include "Logger.h"

void InitConsole();
void CloseConsole();

// NOTE: This needs to be linked to the exe since this gives the agility sdk from windows. It allows us to use newer
// features without being dependent on the windows version runnning on the user machine
// If there is a DXGI_ERROR_SDK_COMPONENT_MISSING or similar, make sure this is being linked correctly to the exe
// Version must match the thirdparty lib version used
// The path must be where the DLLs are found
extern "C"
{
    __declspec(dllexport) extern const UINT D3D12SDKVersion = SSSENGINE_AGILITY_SDK_VERSION;
    __declspec(dllexport) extern const char8_t *D3D12SDKPath = u8".\\Directx12\\D3D12\\";
}

constexpr WCHAR WindowClassName[] = L"SSS Engine";

// LOW_PRIORITY: Check for cpu attributes to ensure minimum specs
//  -> SIMD
//  -> Minimum memory perhaps or just try to allocate and if fails allocate
//  less?
// NOLINTNEXTLINE
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
    /*
     *  TODO: We need to know, using the cmd line args the file path of the project so we can open it
     *  If the user tries to open using the exe this path will not be set, so we can instead close this application and
     *  run the launcher first
     *
     *  TODO: Some of the code here is probably going to be present in multiple windows main function. We should move
     *  everything that is common into a window.h file and have init and possibly other functions to improve. Anything
     *  that is not platform specific should instead be moved into the application class to initialize like audio,
     *  input...
     */
    InitConsole();

    // COM initialization
    HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if(FAILED(hr))
    {
        // OutputDebugStringW(L"COM initialization failed");
        SSSENGINE_LOG_ERROR("COM initialization failed");
        return -1;
    }

    // TODO: Move it another file
    // NOTE: This uses the DIA SDK but not sure it's very useful
    {
        // NOTE: You need to have the dll registered in server to use this debug
        //  From MSDOCS:
        //  To register the dll, open an elevated Developer command prompt window,
        //  and change to the directory that contains the version for your machine
        //  architecture. Enter the command regsvr32 msdia140.dll to register the
        //  COM server.
        //
        // CComPtr<IDiaDataSource> source;
        // if (FAILED(CoCreateInstance(CLSID_DiaSource, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&source))))
        //{
        //    OutputDebugString(L"Failed to create DiaSource instance");
        //}

        // if (FAILED(source->loadDataFromPdb(LR"(Directx12/Directx12.pdb)")))
        //{
        //     OutputDebugString(L"Failed to load data from PDB");
        // }

        // CComPtr<IDiaSession> session;
        // if (FAILED(source->openSession(&session)))
        //{
        //     OutputDebugString(L"Failed to open session");
        // }

        // CComPtr<IDiaSymbol> global;
        // if (FAILED(session->get_globalScope(&global)))
        //{
        //     OutputDebugString(L"Failed to get global scope");
        // }

        // CComPtr<IDiaEnumTables> tables;
        // if (FAILED(session->getEnumTables(&tables)))
        //{
        //     OutputDebugString(L"Failed to get enumerated tables");
        // }

        // CComPtr<IDiaTable> table;
        // ULONG celt;
        // while (SUCCEEDED(tables->Next(1, &table, &celt)) && celt == 1)
        //{
        //     // Do something with each IDiaTable.
        //     BSTR name;
        //     table->get_name(&name);
        //     OutputDebugString(name);
        //     OutputDebugString(L"\n");
        //     table = nullptr;
        // }
    }

    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2); // NOLINT

    SSSEngine::Platform::Win32::WindowClass.cbSize = sizeof(WNDCLASSEX);
    SSSEngine::Platform::Win32::WindowClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    SSSEngine::Platform::Win32::WindowClass.lpfnWndProc = SSSEngine::Platform::Win32::MainWindowProcedure;
    SSSEngine::Platform::Win32::WindowClass.hInstance = hInstance;
    SSSEngine::Platform::Win32::WindowClass.lpszClassName = WindowClassName; // NOLINT(*-pro-bounds-array-to-pointer-decay)
    SSSEngine::Platform::Win32::WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW); // NOLINT
    SSSEngine::Platform::Win32::WindowClass.lpszMenuName = RT_MENU; // NOLINT

    if(RegisterClassExW(&SSSEngine::Platform::Win32::WindowClass) == 0)
    {
        OutputDebugStringW(L"Window class creation failed");
        return -1;
    }

    // TODO: Pass the args
    SSSEngine::Platform::RunApplication(0, nullptr);

    UnregisterClass(WindowClassName, hInstance);
    CloseConsole();
    return 0;
}

void InitConsole()
{
    AllocConsole();
    FILE *stream; // NOLINT(*-init-variables)
    freopen_s(&stream, "CONOUT$", "w", stdout);
    freopen_s(&stream, "CONOUT$", "w", stderr);
    freopen_s(&stream, "CONIN$", "r", stdin);
}

void CloseConsole()
{
    FreeConsole();
}
