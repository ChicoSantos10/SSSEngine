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
#include "Types.h"

#include <cfloat>

namespace SSSEngine::Math::Limits
{
    /**
     * @brief The size in bits of the Type T
     *
     * @tparam T The Type to check the size of
     */
    template<typename T>
    SSSENGINE_GLOBAL constexpr int Bits = sizeof(T) * 8;

    /**
     * @brief A lambda that when instatiated results in a compile time error
     *
     * Used for generic variables to detect when a specialization is not supported yet
     *
     * @tparam T A random type, only used to prevent instantiating it when not needed.
     */
    template<typename T>
    SSSENGINE_GLOBAL auto NotSupportedError = [] { SSSENGINE_STATIC_ASSERT(false, "Type is not supported"); };

    /**
     * @brief The amount of meaningful radix digits for type N
     *
     * @param N The type, representing a number, to check the radix digits
     */
    template<NumberConcept N>
    SSSENGINE_GLOBAL constexpr int Digits = NotSupportedError<N>();

    /**
     * @brief The amount of radix digits meaningful for Integer Types. That is to say the bits that represent the actual
     * number excluding sign and padding bits if present.
     *
     * This means that for unsigned types the value is Bits - 1, otherwise its Bits
     *
     * @see Bits
     *
     * @param N The integer type to check
     */
    template<IntegralConcept N>
    SSSENGINE_GLOBAL constexpr int Digits<N> = IsSigned<N> ? Bits<N> - 1 : Bits<N>;

    /**
     * @brief The amount of radix digits for 32-bit floating point number.
     *
     * This equals the base 2 digits that can be represented without losing precision
     */
    template<>
    SSSENGINE_GLOBAL constexpr int Digits<f32> = FLT_MANT_DIG;

    /**
     * @brief The amount of radix digits for 64-bit floating point number.
     *
     * This equals the base 2 digits that can be represented without losing precision
     */
    template<>
    SSSENGINE_GLOBAL constexpr int Digits<f64> = DBL_MANT_DIG;

    /**
     * @brief The amount of radix digits for 128-bit floating point number.
     *
     * This equals the base 2 digits that can be represented without losing precision
     */
    template<>
    SSSENGINE_GLOBAL constexpr int Digits<long double> = LDBL_MANT_DIG;

    template<NumberConcept N>
    SSSENGINE_GLOBAL constexpr N Max = NotSupportedError<N>();

    /**
     * @brief The maximum number that can be represented by the Integer type N
     */
    template<IntegralConcept N>
    SSSENGINE_GLOBAL constexpr N Max<N> = IsSigned<N> ? ~(N{1} << (Bits<N> - 1)) : ~N{0};

    /**
     * @brief The maximum number that can be represented by a 32-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL constexpr f32 Max<f32> = FLT_MAX;

    /**
     * @brief The maximum number that can be represented by a 64-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL constexpr f64 Max<f64> = DBL_MAX;

    /**
     * @brief The maximum number that can be represented by a 128-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL constexpr f128 Max<f128> = LDBL_MAX;

    template<NumberConcept N>
    SSSENGINE_GLOBAL constexpr N Min = NotSupportedError<N>();

    /**
     * @brief The minimum number that can be represented by the Integer type N
     */
    template<IntegralConcept N>
    SSSENGINE_GLOBAL constexpr N Min<N> = IsSigned<N> ? static_cast<N>(N{1} << (Bits<N> - 1)) : N{0};

    /**
     * @brief The minimum representable number by the floating point number type N
     *
     * Is the negative value of Max<N>
     */
    template<RealConcept N>
    SSSENGINE_GLOBAL constexpr N Min<N> = -Max<N>;

    template<RealConcept N>
    SSSENGINE_GLOBAL constexpr N SmallestPositive = NotSupportedError<N>();

    /**
     * @brief The closest number to 0 possible to represent with a 32-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL constexpr f32 SmallestPositive<f32> = FLT_MIN;

    /**
     * @brief The closest number to 0 possible to represent with a 64-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL constexpr f64 SmallestPositive<f64> = DBL_MIN;

    /**
     * @brief The closest number to 0 possible to represent with a 128-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL constexpr f128 SmallestPositive<f128> = LDBL_MIN;

    template<RealConcept N>
    SSSENGINE_GLOBAL constexpr N Epsilon = NotSupportedError<N>();

    /**
     * @brief The difference between 1.0f and the next number representable by a 32-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL constexpr f32 Epsilon<f32> = FLT_EPSILON;

    /**
     * @brief The difference between 1.0f and the next number representable by a 64-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL constexpr f64 Epsilon<f64> = DBL_EPSILON;

    /**
     * @brief The difference between 1.0f and the next number representable by a 128-bit floating point number
     */
    template<>
    SSSENGINE_GLOBAL constexpr f128 Epsilon<f128> = LDBL_EPSILON;
} // namespace SSSEngine::Math::Limits
