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
