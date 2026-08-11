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
                final = Text::Format<Text::Utf8Encoding>(u8"[ERROR]: {}", message);
                FileSystem::StdError.Write(reinterpret_cast<const void *>(final.CString()), final.Count());
                return;
        }

        FileSystem::StdOut.Write(reinterpret_cast<const void *>(final.CString()), final.Count());
    }

} // namespace SSSEngine::Logging
