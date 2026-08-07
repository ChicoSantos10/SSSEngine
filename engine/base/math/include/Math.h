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
 * @brief File with some common Math functions
 */

#pragma once

#include "Attributes.h"
#include "Bits.h"
#include "Concepts.h"
#include "ConversionTraits.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "Limits.h"
#include "SignTraits.h"
#include "Traits.h"
#include "Types.h"
#include "Swap.h"

namespace SSSEngine::Math
{
    SSSENGINE_FORCE_INLINE
    constexpr auto AsBits(RealConcept auto value) noexcept
    {
        if constexpr(sizeof(value) == 4)
        {
            return BitCopy<u32>(value);
        }
        else if constexpr(sizeof(value) == 8)
        {
            return BitCopy<u64>(value);
        }
        else
        {
            SSSENGINE_NOT_IMPLEMENTED;
        }
    }

    SSSENGINE_FORCE_INLINE
    constexpr auto AsSignedBits(RealConcept auto value) noexcept
    {
        auto num = AsBits(value);
        return static_cast<SignedType<decltype(num)>>(num);
    }

    template<RealConcept R>
    SSSENGINE_GLOBAL
    constexpr u64 ExponentMask = 0;

    template<RealConcept R>
    SSSENGINE_GLOBAL
    constexpr u64 FractionMask = 0;

    template<>
    SSSENGINE_GLOBAL
    constexpr u32 ExponentMask<f32> = 0x7F800000U;

    template<>
    SSSENGINE_GLOBAL
    constexpr u32 FractionMask<f32> = 0x007FFFFFU;

    template<>
    SSSENGINE_GLOBAL
    constexpr u64 ExponentMask<f64> = 0x7FF0000000000000ULL;

    template<>
    SSSENGINE_GLOBAL
    constexpr u64 FractionMask<f64> = 0x000FFFFFFFFFFFFFULL;

    /**
     * @brief Get's the absolute representation of a signed integer
     *
     * @important For signed integer values, due to how the two's complement works, there is no representation for the
     * absolute number of the min. Make sure that in those cases do not pass the minimum possible number otherwise the
     * result will cause overflow. Since overflow on signed integers is undefined this will be too
     *
     * @param num The number to get it's absolute representation
     * @return The absolute value of num
     */
    template<IntegralConcept T>
        requires(IsSigned<T>)
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr T Absolute(T num) noexcept
    {
        SSSENGINE_ASSERT(num != Limits::Min<T>);

        auto y = num >> (Limits::Bits<T> - 1);
        return (num ^ y) - y;
    }

    /**
     * @brief Get's the absolute representation of a floating point number
     *
     * @param num The number to get it's absolute representation
     * @return The absolute value of num
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto Absolute(RealConcept auto num) noexcept
    {
        return num < 0 ? -num : num;
    }

    /**
     * @brief Counts the zero bits from the least significant bit until a one bit is found
     *
     * Example:
     * @code
     * CountRightZeros(8) -> 3
     *
     * Explanation:
     * 8 is 0b1000
     *
     * @tparam Fallback true if the function should fallback to 0 if the mask is 0 or false if the mask is guaranteed to
     * be non zero. If fallback is false and mask is zero result is undefined.
     * @param num The mask to count
     * @return The count if the mask is non zero and 0 if the mask is zero and fallback is true. Undefined if mask is 0
     * and fallback is false
     */
    template<bool Fallback = false>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr u8 CountRightZeros(IntegralConcept auto mask) noexcept
    {
        if constexpr(!Fallback)
        {
            SSSENGINE_ASSERT(mask != 0);
        }

        auto asUnsigned = BitCopy<UnsignedType<decltype(mask)>>(mask);

#ifdef SSSENGINE_MSVC
        byte index;
        auto result = _BitScanForward(&index, asUnsigned);
        if constexpr(Fallback)
        {
            return result == 0 ? 0 : index;
        }
        return index;
#elif SSSENGINE_GCC || SSSENGINE_CLANG
        if constexpr(Fallback)
        {
            return __builtin_ctzg(asUnsigned, 0);
        }
        return __builtin_ctzg(asUnsigned);
#endif
    }

    /**
     * @brief Counts the zero bits from the most significant bit until a one bit is found
     *
     * Example:
     * @code
     * CountLeftZeros(u8(8)) -> 4
     *
     * Explanation:
     * 8 is 0b00001000
     *
     * @tparam Fallback true if the function should fallback to 0 if the mask is 0 or false if the mask is guaranteed to
     * be non zero. If fallback is false and mask is zero result is undefined.
     * @param num The mask to count
     * @return The count if the mask is non zero and 0 if the mask is zero and fallback is true. Undefined if mask is 0
     * and fallback is false
     */
    template<bool Fallback = false>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr char CountLeftZeros(IntegralConcept auto num) noexcept
    {
        if constexpr(!Fallback)
        {
            SSSENGINE_ASSERT(num != 0);
        }
        using Type = decltype(num);

        auto asUnsigned = BitCopy<UnsignedType<Type>>(num);

#ifdef SSSENGINE_MSVC
        byte index;
        auto result = _BitScanReverse(&index, asUnsigned);
        if constexpr(Fallback)
        {
            return result == 0 ? 0 : index;
        }
        return index;
#elif SSSENGINE_GCC || SSSENGINE_CLANG
        if constexpr(Fallback)
        {
            return __builtin_clzg(asUnsigned, 0);
        }
        return __builtin_clzg(asUnsigned);
#endif
    }

    // PERF: This common min and max implementations could be bad for types with expensive copies. This way we do the
    // casting once for both which means we do not need to cast again in case the operator casts it. But if the operator
    // does not need to cast, it will cause 2 copies to happen instead of potentially one. For types like that is
    // probably better to specialize the max and min function Benchmark it

    /**
     * @brief Returns the minimum value between 2 values
     *
     * @tparam T The Type of values to compare
     * @param first A value to compare
     * @param second The other value to compare to
     * @return first if first < second is true; second otherwise
     */
    template<typename T>
        requires OrderableConcept<T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto Min(const T &first, const T &second) noexcept
    {
        return first < second ? first : second;
    }

    /**
     * @brief Computes the minimum of two values
     *
     * The values do not need to be of the same type, in which case they will be casted to their common type.
     *
     * @important This may copy the values if the common type is different than their respective types! Avoid with
     * expensive copies! Also if the comparison is between a signed and unsigned type, if the signed number is negative
     * it will overflow and possibly become the bigger number
     *
     * @see Min(first, second)
     *
     * @param x The first number
     * @param y The seconds number
     * @return The smaller of the two numbers
     */
    template<typename T, typename U>
        requires OrderableWithConcept<T, U>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto CommonMin(const T &x, const U &y) noexcept
    {
        using ReturnType = CommonType<T, U>;
        auto first = static_cast<ReturnType>(x);
        auto second = static_cast<ReturnType>(y);
        return Min(first, second);
    }

    /**
     * @brief Returns the maximum value between 2 values
     *
     * @tparam T Any type that implements the ComparableConcept
     * @param first A value to compare
     * @param second The other value to compare to
     * @return first if first > second is true; second otherwise
     */
    template<typename T>
        requires OrderableConcept<T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto Max(const T &first, const T &second) noexcept
    {
        return first > second ? first : second;
    }

    /**
     * @brief Computes the maximum of two values
     *
     * The values do not need to be of the same type, in which case they will be casted to their common type.
     *
     * @important This may copy the values if the common type is different than their respective types! Avoid with
     * expensive copies! Also if the comparison is between a signed and unsigned type, if the signed number is negative
     * it will overflow and possibly become the bigger number
     *
     * @see Max(first, second)
     *
     * @param x The first number
     * @param y The second number
     * @return The Max(first, second) of the two numbers
     */
    template<typename T, typename U>
        requires OrderableWithConcept<T, U>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto CommonMax(const T &x, const U &y) noexcept
    {
        using ReturnType = CommonType<T, U>;
        auto first = static_cast<ReturnType>(x);
        auto second = static_cast<ReturnType>(y);
        return Max(first, second);
    }

    // TODO: Reword this:
    /**
     * @brief Gets the next integer greater or equal to num
     *
     * @tparam ReturnType The type to return
     * @param num The number to check
     * @return The next integer greater or equal to num
     */
    template<NumberConcept ReturnType = f32, RealConcept T>
    constexpr ReturnType Ceil(T num) noexcept
    {
        SSSENGINE_STATIC_ASSERT(IsAnyType<T, f32, f64>);

        using TruncateType = ConditionalType<IsSameType<T, f32>, i32, i64>;

        auto truncated = TruncateType(num);
        if(truncated < num)
        {
            return ReturnType(truncated + 1);
        }

        return ReturnType(truncated);
    }

    /**
     * @brief Calculates the greatest common divisor
     *
     *
     * @param numerator The first number
     * @param denominator The second number
     * @return The greatest common denominator
     */
    template<UnsignedIntegralConcept T>
    SSSENGINE_PURE SSSENGINE_GLOBAL
    constexpr T GreatestCommonDivisor(T x, T y) noexcept
    {
        if(x == 0)
        {
            return y;
        }

        if(y == 0)
        {
            return x;
        }

        const char shiftNum = CountRightZeros(x);
        x >>= shiftNum;
        const char shiftDen = CountRightZeros(y);
        y >>= shiftDen;

        const char min = Min(shiftNum, shiftDen);
        while(true)
        {
            if(x > y)
            {
                Swap(x, y);
            }

            y -= x;

            if(y == 0)
            {
                return x << min;
            }

            y >>= CountRightZeros(y);
        }
    }

    /**
     * @brief Calculates the greatest common denominator between 2 numbers
     *
     * Both numbers can be positive or negative but must be signed integers. This will cast them into their respective
     * absolute value into an unsigned integer.
     *
     * @param numerator The first number
     * @param denominator The second number
     * @return Return the greatest common denominator
     */
    template<SignedIntegralConcept T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr T GreatestCommonDivisor(T x, T y) noexcept
    {
        using Unsigned = UnsignedType<T>;
        auto unsignedX = static_cast<Unsigned>(Absolute(x));
        auto unsignedY = static_cast<Unsigned>(Absolute(y));

        return GreatestCommonDivisor(unsignedX, unsignedY);
    }

    /**
     * @brief Calculates the least common multiple between two numbers
     *
     * @param x The first number
     * @param y The second number
     * @return The least common multiple between the x and y
     */
    template<IntegralConcept T>
    SSSENGINE_PURE SSSENGINE_GLOBAL
    constexpr auto LeastCommonMultiple(T x, T y) noexcept
    {
        return x / GreatestCommonDivisor(x, y) * y;
    }

    /**
     * @brief Returns the sign of a number
     *
     * @param num The number to get the sign of
     * @return Returns 1 if positive, 0 if 0, -1 if negative */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr int SignOf(NumberConcept auto num) noexcept
    {
        using Type = decltype(num);
        auto value = [num]()
        {
            if constexpr(IsReal<Type>)
            {
                return AsSignedBits(num);
            }
            else
            {
                return num;
            }
        }();
        using UnsignedType = UnsignedType<decltype(value)>;

        static constexpr auto Shift = Limits::Bits<Type> - 1;

        return (value >> Shift) | (-static_cast<UnsignedType>(value) >> Shift);
    };

    /**
     * @brief Checks if a number is a power of 2
     *
     * @param multiple The number to check
     * @return True if the number is a power of 2
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr bool IsPowerOf2(IntegralConcept auto multiple)
    {
        return multiple > 0 && (multiple & (multiple - 1)) == 0;
    }

    /**
     * @brief Calculates the next number that is a multiple
     *
     * @param number The number to get the next multiple of
     * @param multiple The multiple to get. Must be a power of 2
     * @return The next number after that is a multiple
     */
    template<IntegralConcept T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr T NextMultiplePowerOf2(T number, T multiple)
    {
        SSSENGINE_ASSERT(IsPowerOf2(multiple));
        return (number + multiple - 1) & -multiple;
    }

    /**
     * @brief Calculates the number of bits needed to store the value. In others the amount of bits the value occupies
     *
     * @param number The number to calculate
     * @return The number of bits needed to store the value
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr IntegralConcept auto BitWidth(IntegralConcept auto value)
    {
        using Type = decltype(value);
        return Limits::BinaryDigits<Type> - CountLeftZeros(value);
    }

    SSSENGINE_PURE SSSENGINE_GLOBAL
    constexpr f32 Log10(f32 value)
    {
        if(value <= 0)
        {
            // TODO: Proper infinity
            return -1.0f / 0;
        }

        static constexpr float Log10of2 = 0.30102999566f;
        static constexpr f32 P0 = 1.4426950408889634f;
        static constexpr f32 P1 = -0.7213475204444817f;
        static constexpr f32 P2 = 0.4808983469629878f;
        static constexpr f32 P3 = -0.36067376022224085f;
        static constexpr f32 P4 = 0.2885390081777927f;
        static constexpr f32 P5 = -0.2402265069591013f;
        static constexpr f32 P6 = 0.20625209037634344f;

        u32 bits = BitCopy<u32>(value);

        int e = static_cast<int>((bits >> 23) & 0xFF) - 127;

        bits = (bits & 0x007FFFFFU) | (0x3F800000U);

        f32 m = BitCopy<f32>(bits);
        if(m < 0.7071067811865476f)
        {
            m *= 2.0f;
            e -= 1;
        }

        f32 t = m - 1.0f;

        // TODO: Use SIMD fmadd
        f32 log2ofM = t * (P0 + t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * (P5 + t * P6))))));

        return (static_cast<f32>(e) + log2ofM) * Log10of2;
    }

    template<RealConcept R>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr bool IsNaN(R value) noexcept
    {
        auto bits = AsBits(value);

        auto exponent = bits & ExponentMask<R>;
        auto fraction = bits & FractionMask<R>;

        return exponent == ExponentMask<R> && fraction != 0;
    }

} // namespace SSSEngine::Math
