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

#include <format>
#include <iostream>
#include "Attributes.h"
#include "Types.h"
#include "Debug.h"

namespace SSSLogging
{
    using String = std::wstring;

    enum class LogLevel : byte
    {
        Info,
        Warning,
        Error,
    };

    template<typename... Args>
    SSSENGINE_PURE String Format(std::wformat_string<Args...> fmt, Args &&...args)
    {
        return std::format(fmt, std::forward<Args>(args)...);
    }

    void LogConsole(LogLevel level, const String &message)
    {
        switch(level)
        {
            case LogLevel::Info:
                std::wcout << L"[INFO]: " << message << "\n";
                break;
            case LogLevel::Warning:
                std::wcout << L"[WARNING]: " << message << "\n";
                break;
            case LogLevel::Error:
                std::wcerr << L"[ERROR]: " << message << "\n";
                break;
            default:
                SSSENGINE_UNREACHABLE;
        }
    }

    // TODO: Macro for enabling/disabling logs
#define SSSENGINE_LOG_INFO(message, ...)                                                                               \
    LogConsole(SSSLogging::LogLevel::Info, SSSLogging::Format(SSSENGINE_WIDE(message), ##__VA_ARGS__))
#define SSSENGINE_LOG_WARNING(message, ...)                                                                            \
    LogConsole(SSSLogging::LogLevel::Error, SSSLogging::Format(SSSENGINE_WIDE(message), ##__VA_ARGS__))
#define SSSENGINE_LOG_ERROR(message, ...)                                                                              \
    LogConsole(SSSLogging::LogLevel::Error, SSSLogging::Format(SSSENGINE_WIDE(message), ##__VA_ARGS__))
} // namespace SSSLogging
