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
 * @brief Utils time related
 */

#pragma once

#include "Attributes.h"
#include "Types.h"

namespace SSSEngine
{
    // TODO: Create hard types akin to std::chrono::duration

    /**
     * @brief Converts seconds to milliseconds when multiplied
     */
    SSSENGINE_MAYBE_UNUSED constexpr u32 Milliseconds = static_cast<int>(1e3);
    /**
     * @brief Converts seconds to microseconds when multiplied
     */
    SSSENGINE_MAYBE_UNUSED constexpr u32 Microseconds = static_cast<int>(1e6);
    /**
     * @brief Converts seconds to nanoseconds when multiplied
     */
    SSSENGINE_MAYBE_UNUSED constexpr u32 Nanoseconds = static_cast<int>(1e9);

    SSSENGINE_STATIC_ASSERT(1'000 == Milliseconds, "1 second must be 1 000 Miliseconds");
    SSSENGINE_STATIC_ASSERT(1'000'000 == Microseconds, "1 second must be 1 000 000 Microseconds");
    SSSENGINE_STATIC_ASSERT(1'000'000'000 == Nanoseconds, "1 second must be 1 000 000 000 Nanoseconds");

    /**
     * @brief Converts milliseconds into seconds
     *
     * @param milliseconds The number of milliseconds to convert to seconds
     * @return The milliseconds converted to seconds
     */
    consteval f64 operator""_ms(const Size milliseconds)
    {
        return static_cast<f64>(milliseconds) / Milliseconds;
    }

    /**
     * @brief Converts microseconds into seconds
     *
     * @param microseconds The number of microseconds to convert to seconds
     * @return The microseconds converted to seconds
     */
    consteval f64 operator""_us(const Size microseconds)
    {
        return static_cast<f64>(microseconds) / Microseconds;
    }

    /**
     * @brief Converts nanoseconds into seconds
     *
     * @param nanoseconds The number of nanoseconds to convert to seconds
     * @return The nanoseconds converted to seconds
     */
    consteval f64 operator""_ns(const Size nanoseconds)
    {
        return static_cast<f64>(nanoseconds) / Nanoseconds;
    }

    SSSENGINE_STATIC_ASSERT(1_ms == 0.001, "1_ms is 0.001 seconds");
    SSSENGINE_STATIC_ASSERT(1_us == 0.000001, "1_us is 0.000001 seconds");
    SSSENGINE_STATIC_ASSERT(1_ns == 0.000000001, "1_ns is 0.000000001 seconds");
} // namespace SSSEngine
