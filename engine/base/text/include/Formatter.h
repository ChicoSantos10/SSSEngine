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
 * @brief Helper for formatting strings
 */

#pragma once

#include "String.h"
#include "StringView.h"

namespace SSSEngine::Text
{
    template<typename... Args>
    Utf8 Format(Utf8View format, Args &&...args)
    {
        // Find replacement fields: {} which can have an Id and/or a format spec {id:spec}
        // Ignore escape sequence {{ and }} => replaced by {} in the output string
        // Convert the type into string
        // All args and replacement fields must be used

        int a[sizeof...(args)];

    } // namespace SSSEngine::Text
