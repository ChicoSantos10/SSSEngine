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

#include "Attributes.h"
#include "Concepts.h"
#include "EnumHelpers.h"

namespace SSSEngine
{
    /**
     * @brief Joins the bits together by doing a bitwise OR (|)
     *
     * @tparam Integer types like int, uint, byte...
     * @param bits The bits to join
     * @return A new number with the 1 bits from every parameter
     */
    SSSENGINE_FORCE_INLINE constexpr auto Join(IntegralConcept auto... bits)
    {
        return (bits | ...);
    }

    /**
     * @brief Joins the bits together by doing a bitwise OR (|)
     *
     * @param flag The value to join to
     * @param args The flags to join
     * @return A new flag with all bits common to flag and args
     */
    template<EnumConcept T, SameAsConcept<T>... U>
    SSSENGINE_FORCE_INLINE constexpr auto Join(T flag, U... args)
    {
        return static_cast<T>(Join(AsNumber(flag), AsNumber(args)...));
    }

    /**
     * @brief Checks to see if first has all the bits set from second
     *
     * @param first The first number to check
     * @param second The number to check against
     * @return True if all bits are set, false otherwise
     */
    SSSENGINE_FORCE_INLINE constexpr bool HasBitSet(IntegralConcept auto first, IntegralConcept auto second)
    {
        return (first & second) != 0;
    }

    /**
     * @brief Checks to see if first has all the bits set from second
     *
     * The order does not matter
     *
     * e.g:
     * first  -> 0100 1010
     * second -> 1001 1010
     * result -> 0000 1010 != 0 return true
     *
     * @param first A flag to check
     * @param second The other flag to check
     * @return True if all bits are set, false otherwise
     */
    template<EnumConcept Flag>
    SSSENGINE_FORCE_INLINE constexpr bool HasBitSet(Flag first, Flag second)
    {
        return HasBitSet(AsNumber(first), AsNumber(second));
    }

    /**
     * @brief Gets a number without the bits from second.
     *
     * @tparam The number used for first and second
     * @param first The number to check
     * @param bits The values to check against
     * @return A number without the bits that are set in args
     */
    template<IntegralConcept T, SameAsConcept<T>... Bits>
    SSSENGINE_FORCE_INLINE constexpr T WithoutBits(T first, Bits... bits)
    {
        return first & ~Join(bits...);
    }

    /**
     * @brief Gets a number without the bits from second.
     *
     * @param first The number to check
     * @param bits The values to check against
     * @return A number without the bits that are set in args
     */
    template<EnumConcept T, SameAsConcept<T>... Bits>
    SSSENGINE_FORCE_INLINE constexpr T WithoutBits(T first, Bits... bits)
    {
        return static_cast<T>(AsNumber(first) & ~Join(AsNumber(bits)...));
    }

    template<typename To, typename From>
        requires(sizeof(To) == sizeof(From) && TriviallyCopyable<To> && TriviallyCopyable<From>)
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE constexpr To BitCopy(const From &from) noexcept
    {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_GCC | SSSENGINE_CLANG
        return __builtin_bit_cast(To, from);
#endif
    }
} // namespace SSSEngine
