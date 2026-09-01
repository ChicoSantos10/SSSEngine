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

#pragma once

#include "File.h"
#include "Formatter.h"
#include "Logger.h"
#include "String.h"
#include "StringView.h"
#include "Utf8Encoding.h"
#include "StandardFileStream.h"

namespace SSSEngine::Logging
{
    void LogConsole(LogLevel level, Text::StringView<Text::Utf8Encoding> message)
    {
        // TODO: Improve this:
        //  - Receive fmt string + args and format to file sink (stdout or stderr)
        //  - Color

        Text::Utf8 final;
        switch(level)
        {
            case LogLevel::Info:
                final = Text::Format<Text::Utf8Encoding>(u8"[INFO]: {}", message);
                break;
            case LogLevel::Warning:
                final = Text::Format<Text::Utf8Encoding>(u8"[WARNING]: {}", message);
                break;
            case LogLevel::Error:
                final = Text::Format<Text::Utf8Encoding>(u8"\033[31m[ERROR]:\033[0m {}", message);
                FileSystem::StdError.Write(reinterpret_cast<const void *>(final.CString()), final.Count());
                return;
        }

        FileSystem::StdOut.Write(reinterpret_cast<const void *>(final.CString()), final.Count());
    }

} // namespace SSSEngine::Logging
