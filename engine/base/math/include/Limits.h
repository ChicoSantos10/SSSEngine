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
 * @brief Represents the numerous limits for numeric types
 */

#pragma once

#include "Concepts.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "SignTraits.h"
#include "Types.h"

namespace SSSEngine::Math::Limits
{
    /**
     * @brief The size in bits of the Type T
     *
     * @tparam T The Type to check the size of
     */
    template<typename T>
    SSSENGINE_GLOBAL
    constexpr u64 Bits = sizeof(T) * 8;

    /**
     * @brief A lambda that when instatiated results in a compile time error
     *
     * Used for generic variables to detect when a specialization is not supported yet
     *
     * @tparam T A random type, only used to prevent instantiating it when not needed.
     */
    template<typename T>
    SSSENGINE_GLOBAL
    auto NotSupportedError = [] { SSSENGINE_STATIC_ASSERT(false, "Type is not supported"); };

    /**
     * @brief The amount of meaningful radix digits for type N
     *
     * @param N The type, representing a number, to check the radix digits
     */
    template<NumberConcept N>
    SSSENGINE_GLOBAL
    constexpr int BinaryDigits = NotSupportedError<N>();

    /**
     * @brief The amount of radix digits meaningful for Integer Types. That is to say the bits that represent the actual
     * number excluding sign and padding bits if present.
     *
     * This means that for signed types the value is Bits - 1, otherwise its Bits
     *
     * @see Bits
     *
     * @param N The integer type to check
     */
    template<IntegralConcept N>
    SSSENGINE_GLOBAL
    constexpr int BinaryDigits<N> = IsSigned<N> ? Bits<N> - 1 : Bits<N>;

    /**
     * @brief The amount of radix digits for 32-bit floating point number.
     *
     * This equals the base 2 digits that can be represented without losing precision
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr int BinaryDigits<f32> = __FLT_MANT_DIG__;

    /**
     * @brief The amount of radix digits for 64-bit floating point number.
     *
     * This equals the base 2 digits that can be represented without losing precision
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr int BinaryDigits<f64> = __DBL_MANT_DIG__;

    /**
     * @brief The amount of radix digits for 128-bit floating point number.
     *
     * This equals the base 2 digits that can be represented without losing precision
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr int BinaryDigits<f128> = __LDBL_MANT_DIG__;

    template<NumberConcept N>
    SSSENGINE_GLOBAL
    constexpr N Max = NotSupportedError<N>();

    /**
     * @brief The maximum number that can be represented by the Integer type N
     */
    template<IntegralConcept N>
    SSSENGINE_GLOBAL
    constexpr N Max<N> = IsSigned<N> ? ~(N{1} << (Bits<N> - 1)) : ~N{0};

    /**
     * @brief The maximum number that can be represented by a 32-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr f32 Max<f32> = __FLT_MAX__;

    /**
     * @brief The maximum number that can be represented by a 64-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr f64 Max<f64> = __DBL_MAX__;

    /**
     * @brief The maximum number that can be represented by a 128-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr f128 Max<f128> = __LDBL_MAX__;

    template<NumberConcept N>
    SSSENGINE_GLOBAL
    constexpr N Min = NotSupportedError<N>();

    /**
     * @brief The minimum number that can be represented by the Integer type N
     */
    template<IntegralConcept N>
    SSSENGINE_GLOBAL
    constexpr N Min<N> = IsSigned<N> ? static_cast<N>(N{1} << (Bits<N> - 1)) : N{0};

    /**
     * @brief The minimum representable number by the floating point number type N
     *
     * Is the negative value of Max<N>
     */
    template<FloatingPointConcept N>
    SSSENGINE_GLOBAL
    constexpr N Min<N> = -Max<N>;

    template<FloatingPointConcept N>
    SSSENGINE_GLOBAL
    constexpr N SmallestPositive = NotSupportedError<N>();

    /**
     * @brief The closest number to 0 possible to represent with a 32-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr f32 SmallestPositive<f32> = __FLT_MIN__;

    /**
     * @brief The closest number to 0 possible to represent with a 64-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr f64 SmallestPositive<f64> = __DBL_MIN__;

    /**
     * @brief The closest number to 0 possible to represent with a 128-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr f128 SmallestPositive<f128> = __LDBL_MIN__;

    template<FloatingPointConcept N>
    SSSENGINE_GLOBAL
    constexpr N Epsilon = NotSupportedError<N>();

    /**
     * @brief The difference between 1.0f and the next number representable by a 32-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr f32 Epsilon<f32> = __FLT_EPSILON__;

    /**
     * @brief The difference between 1.0f and the next number representable by a 64-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr f64 Epsilon<f64> = __DBL_EPSILON__;

    /**
     * @brief The difference between 1.0f and the next number representable by a 128-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr f128 Epsilon<f128> = __LDBL_EPSILON__;

    /**
     * @brief The max amount of digits base 10 to write the biggest number of Type N
     *
     * @param N The type, representing a number, to check the decimal digits
     */
    template<NumberConcept N>
    SSSENGINE_GLOBAL
    constexpr int DecimalDigits = NotSupportedError<N>();

    /**
     * @brief The max amount of digits base 10 to write the biggest u8 number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr int DecimalDigits<u8> = 3;

    /**
     * @brief The max amount of digits base 10 to write the biggest i8 number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr int DecimalDigits<i8> = 3;

    /**
     * @brief The max amount of digits base 10 to write the biggest u16 number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr int DecimalDigits<u16> = 5;

    /**
     * @brief The max amount of digits base 10 to write the biggest i16 number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr int DecimalDigits<i16> = 5;

    /**
     * @brief The max amount of digits base 10 to write the biggest u32 number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr int DecimalDigits<u32> = 10;

    /**
     * @brief The max amount of digits base 10 to write the biggest i32 number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr int DecimalDigits<i32> = 10;

    /**
     * @brief The max amount of digits base 10 to write the biggest u64 number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr int DecimalDigits<u64> = 20;

    /**
     * @brief The max amount of digits base 10 to write the biggest i64 number
     */
    template<>
    SSSENGINE_GLOBAL
    constexpr int DecimalDigits<i64> = 19;

} // namespace SSSEngine::Math::Limits
