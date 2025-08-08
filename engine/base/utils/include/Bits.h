/*  SSS Engine
    Copyright (C) 2024  Francisco Santos

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "Attributes.h"
#include "Concepts.h"

namespace SSSEngine
{
    /**
     * @brief Joins the bits together by doing a bitwise OR (|)
     *
     * @tparam Integer types like int, uint, byte...
     * @param bits The bits to join
     * @return A new number with the 1 bits from every parameter
     */
    SSSENGINE_FORCE_INLINE constexpr auto Join(IntegralConcept auto... bits) { return (bits | ...); }

    /**
     * @brief Checks to see if first has all the bits set from second
     *
     * @param first The first number to check
     * @param second The number to check against
     * @return True if all bits are set, false otherwise
     */
    SSSENGINE_FORCE_INLINE constexpr bool HasBitSet(IntegralConcept auto first, IntegralConcept auto second)
    {
        return first & second;
    }

    /**
     * @brief Gets a number without the bits from second.
     *
     * @tparam The number used for first and second
     * @param first The number to check
     * @param bits The values to check against
     * @return A number without the bits that are set in args
     */
    template<IntegralConcept T, IntegralConcept... Bits>
        requires EqualTypesConcept<T, Bits...>
    SSSENGINE_FORCE_INLINE constexpr T WithoutBits(T first, Bits... bits)
    {
        return first & ~Join(bits...);
    }
} // namespace SSSEngine
