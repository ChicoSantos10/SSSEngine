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

// NOTE: Windows.h needs to be included for dbghelp to work.
#include "windows.h"
#include <dbghelp.h>
#include <sstream>
#include "Debug.h"

SSSENGINE_LIB("dbghelp.lib")

// LOW_PRIORITY: This has no safety measures aside from looking until \0
wchar_t *Trim(wchar_t *string)
{
    wchar_t *lastDivisor = nullptr;
    for(auto c = string; *c != L'\0'; c++)
    {
        if(*c == L'/' || *c == L'\\')
        {
            lastDivisor = c;
        }
    }

    if(lastDivisor != nullptr)
        return ++lastDivisor;

    return string;
}

void ReportAssertionFailure(const wchar_t *message, const wchar_t *file, unsigned line)
{
    // TODO: Clean up -> Make use of the error code + Create a proper way to show debug codes + only run code when
    //  debug is on
    auto currentOptions = SymSetOptions(SYMOPT_LOAD_LINES);

    DWORD error = 0;
    // TODO: RAII on the initialization
    if(!SymInitialize(GetCurrentProcess(), nullptr, true))
        error = GetLastError();

    // TODO: Message still needs the file and line as well as it could have the stack trace
    MessageBeep(MB_ICONERROR);
    const auto process = GetCurrentProcess();
    const auto thread = GetCurrentThread();

    // Get function names and their respective line number
    {
        //     PVOID backtrace[UCHAR_MAX];
        //
        //     CaptureStackBackTrace(0, UCHAR_MAX, backtrace, nullptr); // NOLINT(*-pro-bounds-array-to-pointer-decay)
        //
        //     for (auto i = 0; backtrace[i]; i++)
        //     {
        //         auto address = reinterpret_cast<DWORD64>(backtrace[i]);
        //
        //         char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        //         auto symbolInfo = reinterpret_cast<PSYMBOL_INFO>(buffer);
        //         symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
        //         symbolInfo->MaxNameLen = MAX_SYM_NAME;
        //         if (SymFromAddr(process,
        //                         address,
        //                         nullptr,
        //                         symbolInfo
        //         ))
        //         {
        //             auto symbolName = reinterpret_cast<LPCTSTR>(symbolInfo->Name);
        //             IMAGEHLP_LINE64 currentLine;
        //             currentLine.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        //             DWORD displacement;
        //
        //             if (SymGetLineFromAddr64(process, symbolInfo->Address, &displacement, &currentLine))
        //             {
        //                 // SymGetLineFromAddr64 returned success
        //                 auto filename = currentLine.FileName;
        //                 auto lineNumber = currentLine.LineNumber;
        //
        //                 MessageBox(nullptr,
        //                            SSSENGINE_WIDE_STRING(lineNumber),
        //                            SSSENGINE_WIDE_STRING(filename),
        //                            MB_ICONEXCLAMATION | MB_OK
        //                 );
        //             }
        //             else
        //             {
        //                 // SymGetLineFromAddr64 failed
        //                 error = GetLastError();
        //                 std::cout << error << std::endl;
        //             }
        //         }
        //         else
        //             error = GetLastError();
        //     }
    }

    STACKFRAME frame;
    CONTEXT context{.ContextFlags = CONTEXT_FULL};
    RtlCaptureContext(&context);
    // NOTE: This sets the current stack frame context meaning the function, line, etc. of the RtlCaptureContext above
    frame.AddrPC.Offset = context.Rip;
    frame.AddrPC.Mode = AddrModeFlat;
    frame.AddrFrame.Offset = context.Rbp;
    frame.AddrFrame.Mode = AddrModeFlat;
    frame.AddrStack.Offset = context.Rsp;
    frame.AddrStack.Mode = AddrModeFlat;

    // LOW_PRIORITY: Custom stream
    std::wstringstream ss;
    ss << SSSENGINE_WIDE_STRING(ERROR FOUND AT) << L" " << file << SSSENGINE_WIDE_STRING( :) << L" " << line
       << L"with message: " << message << L"\n";

    while(StackWalk64(
        IMAGE_FILE_MACHINE_AMD64, process, thread, &frame, &context, nullptr, SymFunctionTableAccess64, SymGetModuleBase64, nullptr))
    {
        // LOW_PRIORITY: Trim names to be better understandable
        // LOW_PRIORITY: No need to copy the names to variables

        DWORD64 functionAddress = frame.AddrPC.Offset;

        DWORD64 moduleBase = SymGetModuleBase(process, frame.AddrPC.Offset);
        auto e = GetLastError();
        std::wstringstream stream;
        stream << e << L"\n";
        OutputDebugString(stream.str().c_str());
        WCHAR moduleBuff[MAX_PATH];
        if(!moduleBase || !GetModuleFileName(reinterpret_cast<HINSTANCE>(moduleBase), moduleBuff, MAX_PATH))
        {
            // auto e = GetLastError();
            // std::wstringstream stream;
            // stream << e << L"\n";
            // OutputDebugString(stream.str().c_str());
        }
        LPWSTR moduleName = Trim(moduleBuff);

        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        auto symbolInfo = reinterpret_cast<PSYMBOL_INFO>(buffer);
        symbolInfo->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbolInfo->MaxNameLen = MAX_SYM_NAME;
        if(!SymFromAddr(process, functionAddress, nullptr, symbolInfo))
        {
            OutputDebugString(L"No SymFromAddr\n");
        }
        LPWSTR functionName = Trim(symbolInfo->Name);

        DWORD offset = 0;
        IMAGEHLP_LINE lineInfo;
        lineInfo.SizeOfStruct = sizeof(lineInfo);

        if(!SymGetLineFromAddr(process, frame.AddrPC.Offset, &offset, &lineInfo))
        {
            OutputDebugString(L"No SymGetLineFromAddr\n");
        }
        PWSTR fileName = Trim(lineInfo.FileName);
        DWORD lineNumber = lineInfo.LineNumber;

        ss << moduleName << L": " << fileName << L" " << functionName << L" at " << lineNumber << L"\n\n";
    }

    MessageBox(nullptr, ss.str().c_str(), L"Assertion Failed", MB_OK | MB_ICONERROR);

    SymCleanup(GetCurrentProcess());
}
