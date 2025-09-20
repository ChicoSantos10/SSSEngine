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

#include <cstdint>
#include "Debug.h"

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;
using f128 = long double;
using byte = unsigned char;

using min8i = int_least8_t;
using min16i = int_least16_t;
using min32i = int_least32_t;
using min64i = int_least64_t;
using min8u = uint_least8_t;
using min16u = uint_least16_t;
using min32u = uint_least32_t;
using min64u = uint_least64_t;

using fast8i = int_fast8_t;
using fast16i = int_fast16_t;
using fast32i = int_fast32_t;
using fast64i = int_fast64_t;
using fast8u = uint_fast8_t;
using fast16u = uint_fast16_t;
using fast32u = uint_fast32_t;
using fast64u = uint_fast64_t;

using maxint = intmax_t;
using intptr = intptr_t;
using maxuint = uintmax_t;
using uintptr = uintptr_t;

using functionPtr = void (*)();
using objPtr = void *;

#ifdef SSSENGINE_MSVC
using Size = size_t;
#else
using Size = std::size_t;
#endif // SSSENGINE_MSVC

// Size assertions
SSSENGINE_STATIC_ASSERT(sizeof(u8) == 1, "u8 must be 1 byte")
SSSENGINE_STATIC_ASSERT(sizeof(u16) == 2, "u16 must be 2 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(u32) == 4, "u32 must be 4 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(u64) == 8, "u64 must be 8 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(i8) == 1, "i8 must be 1 byte")
SSSENGINE_STATIC_ASSERT(sizeof(i16) == 2, "i16 must be 2 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(i32) == 4, "i32 must be 4 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(i64) == 8, "i64 must be 8 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(f32) == 4, "f32 must be 4 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(f64) == 8, "f64 must be 8 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(byte) == 1, "byte must be 1 byte")
