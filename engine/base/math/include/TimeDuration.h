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

#include "Concepts.h"
#include "Math.h"
#include "Quantity.h"
#include "Types.h"
#include "Ratio.h"

namespace SSSEngine
{
    /**
     * @class DurationType
     * @brief Empty struct to work as a tag for Time Duration Quantities
     *
     * @see Math::Quantity
     *
     */
    struct DurationType
    {
    };

    template<NumberConcept T, Math::RatioConcept Ratio>
    using Duration = Math::Quantity<DurationType, T, Ratio>;

    using Nanoseconds = Duration<i64, Math::Nano>;
    using Microseconds = Duration<i64, Math::Micro>;
    using Milliseconds = Duration<i64, Math::Milli>;
    using Seconds = Duration<i64, Math::Ratio<1>>;

    /**
     * @brief Literal to get a duration of type seconds
     *
     * @param seconds The number of seconds to get
     * @return The seconds type with seconds
     */
    consteval Seconds operator""_s(const unsigned long long seconds)
    {
        return Seconds{static_cast<i64>(seconds)};
    }

    /**
     * @brief Literal to get a duration of type milliseconds
     *
     * @param milliseconds The number of milliseconds to get
     * @return The milliseconds type with milliseconds
     */
    consteval Milliseconds operator""_ms(const unsigned long long milliseconds)
    {
        return Milliseconds{static_cast<i64>(milliseconds)};
    }

    /**
     * @brief Literal to get a duration of type microseconds
     *
     * @param microseconds The number of microseconds to get
     * @return The microseconds type with microseconds
     */
    consteval Microseconds operator""_us(const unsigned long long microseconds)
    {
        return Microseconds{static_cast<i64>(microseconds)};
    }

    /**
     * @brief Literal to get a duration of type nanoseconds
     *
     * @param nanoseconds The number of nanoseconds to get
     * @return The nanoseconds type with nanoseconds
     */
    consteval Nanoseconds operator""_ns(const unsigned long long nanoseconds)
    {
        return Nanoseconds{static_cast<i64>(nanoseconds)};
    }
} // namespace SSSEngine
