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

#include "Debug.h"

#ifdef SSSENGINE_WIN32
using i64 = signed long long;
using u64 = unsigned long long;
#else
using i64 = signed long;
using u64 = unsigned long;
#endif

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using f32 = float;
using f64 = double;
using f128 = long double;
using byte = unsigned char;

using min8i = i8;
using min16i = i16;
using min32i = i32;
using min64i = i64;
using min8u = u8;
using min16u = u16;
using min32u = u32;
using min64u = u64;

using fast8i = i8;
using fast16i = i32;
using fast32i = i32;
using fast64i = i64;
using fast8u = u8;
using fast16u = u32;
using fast32u = u32;
using fast64u = u64;

using maxint = i64;
using maxuint = u64;

using functionPtr = void (*)();
using objPtr = void *;

using Size = __SIZE_TYPE__;

using char8 = char8_t;
using char16 = char16_t;
using char32 = char32_t;

using ptrdiff = decltype(static_cast<int *>(nullptr) - static_cast<int *>(nullptr));
using uintptr = u64;
using intptr = i64;

// Size assertions
SSSENGINE_STATIC_ASSERT(sizeof(u8) == 1, "u8 must be 1 byte");
SSSENGINE_STATIC_ASSERT(sizeof(u16) == 2, "u16 must be 2 bytes");
SSSENGINE_STATIC_ASSERT(sizeof(u32) == 4, "u32 must be 4 bytes");
SSSENGINE_STATIC_ASSERT(sizeof(u64) == 8, "u64 must be 8 bytes");
SSSENGINE_STATIC_ASSERT(sizeof(i8) == 1, "i8 must be 1 byte");
SSSENGINE_STATIC_ASSERT(sizeof(i16) == 2, "i16 must be 2 bytes");
SSSENGINE_STATIC_ASSERT(sizeof(i32) == 4, "i32 must be 4 bytes");
SSSENGINE_STATIC_ASSERT(sizeof(i64) == 8, "i64 must be 8 bytes");
SSSENGINE_STATIC_ASSERT(sizeof(f32) == 4, "f32 must be 4 bytes");
SSSENGINE_STATIC_ASSERT(sizeof(f64) == 8, "f64 must be 8 bytes");
SSSENGINE_STATIC_ASSERT(sizeof(byte) == 1, "byte must be 1 byte");
SSSENGINE_STATIC_ASSERT(sizeof(uintptr) == sizeof(void *), "uintptr should be the size of a pointer");
SSSENGINE_STATIC_ASSERT(sizeof(intptr) == sizeof(void *), "intptr should be the size of a pointer");
