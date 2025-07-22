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

#include <cstdio>
#include <exception>
#include <iostream>
#include <ostream>
#include <windows.h>
#include <wrl/client.h>
#include <xinput.h>
#include "Application.h"
#include "Win32Window.h"
#include "xapo.h"
#include "xapofx.h"
#include "xaudio2.h"
#include "xaudio2fx.h"

// Timing
#include "Timer.h"

// Debugs
#include <dbghelp.h>
#include <dia2.h>
#include "Debug.h"
#include "Logger.h"

using std::exception;

void InitConsole();
void CloseConsole();

// NOTE: For some reason this needs to be on main
extern "C"
{
    __declspec(dllexport) extern const UINT D3D12SDKVersion = 610;
    __declspec(dllexport) extern const char8_t *D3D12SDKPath = u8".\\Directx12\\D3D12\\";
}

// SSSENGINE_LIB("dbghelp.lib")

constexpr WCHAR WindowClassName[] = L"SSS Engine";

// LOW_PRIORITY: Check for cpu attributes to ensure minimum specs
//  -> SIMD
//  -> Minimum memory perhaps or just try to allocate and if fails allocate
//  less?
// NOLINTNEXTLINE
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
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

    SSSWin32::WindowClass.cbSize = sizeof(WNDCLASSEX);
    SSSWin32::WindowClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    SSSWin32::WindowClass.lpfnWndProc = SSSWin32::MainWindowProcedure;
    SSSWin32::WindowClass.hInstance = hInstance;
    SSSWin32::WindowClass.lpszClassName = WindowClassName; // NOLINT(*-pro-bounds-array-to-pointer-decay)
    SSSWin32::WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW); // NOLINT
    SSSWin32::WindowClass.lpszMenuName = RT_MENU; // NOLINT

    if(RegisterClassExW(&SSSWin32::WindowClass) == 0)
    {
        OutputDebugStringW(L"Window class creation failed");
        return -1;
    }

    SSSEngine::Application app;
    app.Run();

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

void CloseConsole() { FreeConsole(); }
