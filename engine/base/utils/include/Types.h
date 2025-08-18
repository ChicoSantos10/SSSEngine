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

using size = size_t;

/**
 * @brief Suffix to denote bytes
 *
 *     10_B represents 10 bytes
 *
 * @param n The number of bytes
 * @return The number of bytes
 */
consteval size operator""_B(const size n)
{
    return n;
}

/**
 * @brief Suffix to denote Kibibytes
 * A Kibibyte is 1024 bytes
 *
 * @param n The number of Kibibytes
 * @return The size in bytes of the Kibibytes
 */
consteval size operator""_KiB(const size n)
{
    return n * 1024_B;
}

/**
 * @brief Suffix to denote Mebibytes
 * A Mebibyte is 1024 Kibibytes
 *
 * @param n The number of Mebibytes
 * @return The size in bytes of the Mebibytes
 */
consteval size operator""_MiB(const size n)
{
    return n * 1024_KiB;
}

/**
 * @brief Suffix to denote Gibibytes
 * A Gibibyte is 1024 Mebibytes
 *
 * @param n The number of Gibibytes
 * @return The size in bytes of the Gibibytes
 */
consteval size operator""_GiB(const size n)
{
    return n * 1024_MiB;
}

/**
 * @brief Suffix to denote Tebibytes
 * A Tebibyte is 1024 Gibibytes
 *
 * @param n The number of Gibibytes
 * @return The size in bytes of the Gibibytes
 */
consteval size operator""_TiB(const size n)
{
    return n * 1024_GiB;
}

// INVESTIGATE: This may be confusing to have both
/**
 * @brief Suffix to denote Kilobytes
 * A Kilobyte is 1000 bytes
 *
 * @param n The number of Kilobytes
 * @return The size in bytes of the Kilobytes
 */
consteval size operator""_KB(const size n)
{
    return n * 1000_B;
}

/**
 * @brief Suffix to denote Megabytes
 * A Megabyte is 1000 Kilobytes
 *
 * @param n The number of Megabytes
 * @return The size in bytes of the Megabytes
 */
consteval size operator""_MB(const size n)
{
    return n * 1000_KB;
}

/**
 * @brief Suffix to denote Gigabytes
 * A Gigabyte is 1000 Megabytes
 *
 * @param n The number of Gigabytes
 * @return The size in bytes of the Gigabytes
 */
consteval size operator""_GB(const size n)
{
    return n * 1000_MB;
}

/**
 * @brief Suffix to denote Terabytes
 * A Terabyte is 1000 Gigabytes
 *
 * @param n The number of Terabytes
 * @return The size in bytes of the Terabytes
 */
consteval size operator""_TB(const size n)
{
    return n * 1000_GB;
}

// Size assertions
SSSENGINE_STATIC_ASSERT(sizeof(u8) == 1_B, "u8 must be 1 byte")
SSSENGINE_STATIC_ASSERT(sizeof(u16) == 2_B, "u16 must be 2 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(u32) == 4_B, "u32 must be 4 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(u64) == 8_B, "u64 must be 8 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(i8) == 1_B, "i8 must be 1 byte")
SSSENGINE_STATIC_ASSERT(sizeof(i16) == 2_B, "i16 must be 2 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(i32) == 4_B, "i32 must be 4 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(i64) == 8_B, "i64 must be 8 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(f32) == 4_B, "f32 must be 4 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(f64) == 8_B, "f64 must be 8 bytes")
SSSENGINE_STATIC_ASSERT(sizeof(byte) == 1_B, "byte must be 1 byte")
