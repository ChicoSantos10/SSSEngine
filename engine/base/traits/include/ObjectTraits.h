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
 * @brief Object representation
 */

#pragma once

#include "HelperMacros.h"

namespace SSSEngine
{
    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool HasUniqueObjectRepresentation = __has_unique_object_representations(T);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsObject = __is_object(T);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsClass = __is_class(T);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsUnion = __is_union(T);

} // namespace SSSEngine
