/**
 * @file
 * @brief
 */

#pragma once

#include "File.h"
#include "HelperMacros.h"

#include <unistd.h>

namespace SSSEngine::FileSystem
{
    SSSENGINE_GLOBAL
    File<FilePermissions::Write> StdOut(STDOUT_FILENO);
    SSSENGINE_GLOBAL
    File<FilePermissions::Read> StdIn(STDIN_FILENO);
    SSSENGINE_GLOBAL
    File<FilePermissions::Write> StdError(STDERR_FILENO);
} // namespace SSSEngine::FileSystem
