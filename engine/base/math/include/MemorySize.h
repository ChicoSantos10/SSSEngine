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
 * @brief File with memory types
 */

#pragma once

#include "Quantity.h"
#include "Ratio.h"

namespace SSSEngine::Math
{
    struct MemorySize
    {
    };

    template<RatioConcept P>
    using Memory = Quantity<MemorySize, Size, P>;

    using Bytes = Memory<Ratio<1>>;
    using KiloBytes = Memory<Kilo>;
    using MegaBytes = Memory<Mega>;
    using GigaBytes = Memory<Giga>;
    using TeraBytes = Memory<Tera>;
    using KibiBytes = Memory<Ratio<1024>>;
    using MebiBytes = Memory<Ratio<1'048'576>>;
    using GibiBytes = Memory<Ratio<1'073'741'824>>;
    using TebiBytes = Memory<Ratio<1'099'512'000'000>>;

    /**
     * @brief Suffix to denote bytes
     *
     *     10_B represents 10 bytes
     *
     * @param n The number of bytes
     * @return The number of bytes
     */
    consteval Bytes operator""_B(const unsigned long long n)
    {
        return Bytes{n};
    }

    /**
     * @brief Suffix to denote Kibibytes
     * A Kibibyte is 1024 bytes
     *
     * @param n The number of Kibibytes
     * @return The unsigned long long in bytes of the Kibibytes
     */
    consteval KibiBytes operator""_KiB(const unsigned long long n)
    {
        return KibiBytes{n};
    }

    /**
     * @brief Suffix to denote Mebibytes
     * A Mebibyte is 1024 Kibibytes
     *
     * @param n The number of Mebibytes
     * @return The unsigned long long in bytes of the Mebibytes
     */
    consteval MebiBytes operator""_MiB(const unsigned long long n)
    {
        return MebiBytes{n};
    }

    /**
     * @brief Suffix to denote Gibibytes
     * A Gibibyte is 1024 Mebibytes
     *
     * @param n The number of Gibibytes
     * @return The unsigned long long in bytes of the Gibibytes
     */
    consteval GibiBytes operator""_GiB(const unsigned long long n)
    {
        return GibiBytes{n};
    }

    /**
     * @brief Suffix to denote Tebibytes
     * A Tebibyte is 1024 Gibibytes
     *
     * @param n The number of Gibibytes
     * @return The unsigned long long in bytes of the Gibibytes
     */
    consteval TebiBytes operator""_TiB(const unsigned long long n)
    {
        return TebiBytes{n};
    }

    /**
     * @brief Suffix to denote Kilobytes
     * A Kilobyte is 1000 bytes
     *
     * @param n The number of Kilobytes
     * @return The unsigned long long in bytes of the Kilobytes
     */
    consteval KiloBytes operator""_KB(const unsigned long long n)
    {
        return KiloBytes{n};
    }

    /**
     * @brief Suffix to denote Megabytes
     * A Megabyte is 1000 Kilobytes
     *
     * @param n The number of Megabytes
     * @return The unsigned long long in bytes of the Megabytes
     */
    consteval MegaBytes operator""_MB(const unsigned long long n)
    {
        return MegaBytes{n};
    }

    /**
     * @brief Suffix to denote Gigabytes
     * A Gigabyte is 1000 Megabytes
     *
     * @param n The number of Gigabytes
     * @return The unsigned long long in bytes of the Gigabytes
     */
    consteval GigaBytes operator""_GB(const unsigned long long n)
    {
        return GigaBytes{n};
    }

    /**
     * @brief Suffix to denote Terabytes
     * A Terabyte is 1000 Gigabytes
     *
     * @param n The number of Terabytes
     * @return The unsigned long long in bytes of the Terabytes
     */
    consteval TeraBytes operator""_TB(const unsigned long long n)
    {
        return TeraBytes{n};
    }

} // namespace SSSEngine::Math
