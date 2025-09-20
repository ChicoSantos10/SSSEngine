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

#include "HelperMacros.h"
#include "Attributes.h"

namespace SSSEngine::Platform
{
// TODO: This is a temporary macro. Remove this once we have UTF-8 for the title
#define SSSENGINE_WINDOW_TITLE SSSEngine

#ifdef SSSENGINE_WIN32
    using WindowTitle = const wchar_t *;
    SSSENGINE_MAYBE_UNUSED constexpr WindowTitle MainWindowName = SSSENGINE_WIDE_STRING(WINDOW_TITLE);
#else
    using WindowTitle = const char *;
    SSSENGINE_MAYBE_UNUSED constexpr WindowTitle MainWindowName = SSSENGINE_STRING(WINDOW_TITLE);
#endif

#undef SSSENGINE_WINDOW_TITLE

    // TODO: Here we can receive an actual array
    void RunApplication(int argc, char *argv[]);
} // namespace SSSEngine::Platform
