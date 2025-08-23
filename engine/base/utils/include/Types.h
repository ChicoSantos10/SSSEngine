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

using Size = size_t;

/**
 * @brief How many bits in a byte
 */
constexpr byte Bits = 8;

/**
 * @brief Converts n bits into bytes
 *
 * Since every Byte must always have 8 bits, n must be a multiple of 8
 *
 * @param n The amount of bits to convert
 * @return The amount of bytes corresponding with the n bits
 */
consteval Size operator""_b(const Size n)
{
    SSSENGINE_ASSERT(n % 8 == 0);
    return n / Bits;
}

/**
 * @brief Suffix to denote bytes
 *
 *     10_B represents 10 bytes
 *
 * @param n The number of bytes
 * @return The number of bytes
 */
consteval Size operator""_B(const Size n)
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
consteval Size operator""_KiB(const Size n)
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
consteval Size operator""_MiB(const Size n)
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
consteval Size operator""_GiB(const Size n)
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
consteval Size operator""_TiB(const Size n)
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
consteval Size operator""_KB(const Size n)
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
consteval Size operator""_MB(const Size n)
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
consteval Size operator""_GB(const Size n)
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
consteval Size operator""_TB(const Size n)
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
// Literal operators
SSSENGINE_STATIC_ASSERT((1_B) == 1, "1_B is 1 byte")
SSSENGINE_STATIC_ASSERT((8_b) == 1_B, "8_b is 1 byte")
SSSENGINE_STATIC_ASSERT((1_KiB) == 1024_B, "1_Kib is 1024 bytes")
SSSENGINE_STATIC_ASSERT((1_MiB) == 1024_KiB, "1_MiB is 1024 Kibibytes")
SSSENGINE_STATIC_ASSERT((1_GiB) == 1024_MiB, "1_GiB is 1024 Mebibytes")
SSSENGINE_STATIC_ASSERT((1_TiB) == 1024_GiB, "1_TiB is 1024 Gibibytes")
SSSENGINE_STATIC_ASSERT((1_KB) == 1000_B, "1_Kb is 1000 bytes")
SSSENGINE_STATIC_ASSERT((1_MB) == 1000_KB, "1_MB is 1000 Kilobytes")
SSSENGINE_STATIC_ASSERT((1_GB) == 1000_MB, "1_GB is 1000 Megabytes")
SSSENGINE_STATIC_ASSERT((1_TB) == 1000_GB, "1_TB is 1000 Gigabytes")
