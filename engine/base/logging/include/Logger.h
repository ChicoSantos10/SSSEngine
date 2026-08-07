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
 * @brief File with logging related internals
 * Is only compiled when SSSENGINE_LOGGING is defined
 */

#pragma once

#ifdef SSSENGINE_LOGGING

    #include "Attributes.h"
    #include "Types.h"
    #include "Debug.h"
    #include "String.h"

/**
 * @namespace
 * @brief Logging namespace
 */
namespace SSSEngine::Logging
{
    using String = Text::Utf8;

    enum class LogLevel : u8
    {
        Info,
        Warning,
        Error,
    };

    // TODO: This should be replaced by just having a logger which should be responsible for knowing how to log (file,
    // console, ...)
    SSSENGINE_GLOBAL
    void LogConsole(LogLevel level, const String &message)
    {
        switch(level)
        {
            using enum LogLevel;
            // TODO: OS Logging
            case Info:
                // std::wcout << L"[INFO]: " << message << "\n";
                break;
            case Warning:
                // std::wcout << L"[WARNING]: " << message << "\n";
                break;
            case Error:
                // std::wcerr << L"[ERROR]: " << message << "\n";
                break;
            default:
                SSSENGINE_UNREACHABLE;
        }
    }

    // INVESTIGATE: Should we just disable the macros when no logging is set?
    // Pros:
    //  - Can make sure that logging still happens for certain things while allowing to disable the extra ones
    // Cons:
    //  - Needs to build logging always
    //  - Can lead to mistakes where calling logging directly instead of macros leaking logs when not intending
    #define SSSENGINE_LOG_INFO(message, ...)                                                                           \
        LogConsole(SSSEngine::Logging::LogLevel::Info,                                                                 \
                   SSSEngine::Text::Format<SSSEngine::Text::Utf8Encoding>(SSSENGINE_UTF8(message), ##__VA_ARGS__))
    #define SSSENGINE_LOG_WARNING(message, ...)                                                                        \
        LogConsole(SSSEngine::Logging::LogLevel::Error,                                                                \
                   SSSEngine::Text::Format<SSSEngine::Text::Utf8Encoding>(SSSENGINE_UTF8(message), ##__VA_ARGS__))
    #define SSSENGINE_LOG_ERROR(message, ...)                                                                          \
        LogConsole(SSSEngine::Logging::LogLevel::Error,                                                                \
                   SSSEngine::Text::Format<SSSEngine::Text::Utf8Encoding>(SSSENGINE_UTF8(message), ##__VA_ARGS__))
} // namespace SSSEngine::Logging

#else
    #define SSSENGINE_LOG_INFO(message, ...)
    #define SSSENGINE_LOG_WARNING(message, ...)
    #define SSSENGINE_LOG_ERROR(message, ...)
#endif
