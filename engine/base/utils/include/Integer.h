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

#include "Bits.h"
#include "Concepts.h"
#include "SignTraits.h"

namespace SSSEngine
{
    template<typename T>
    struct IntTraits;

    template<IntegralConcept Int>
    struct IntTraits<Int>
    {
        using Type = Int;

        /**
         * @brief The maximum number that can be represented by the Integer type N
         */
        static constexpr Int Max = IsSigned<Int> ? ~(Int{1} << (Bits<Int> - 1)) : ~Int{0};

        /**
         * @brief The minimum number that can be represented by the Integer type N
         */
        static constexpr Int Min = IsSigned<Int> ? static_cast<Int>(Int{1} << (Bits<Int> - 1)) : Int{0};

        /**
         * @brief The amount of meaningful base 2 digits for type N
         */
        static constexpr u32 BinaryDigits = IsSigned<Int> ? Bits<Int> - 1 : Bits<Int>;

        /**
         * @brief The amount of meaningful base 10 digits for type N
         */
        static constexpr u32 DecimalDigits = BinaryDigits * 0.301029995664;
    };

} // namespace SSSEngine
