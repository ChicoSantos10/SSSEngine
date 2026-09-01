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
 * @brief Traits for IEEE754 floating point numbers
 */

#pragma once

#include "Attributes.h"
#include "Concepts.h"
#include "Debug.h"
#include "Bits.h"
#include "Types.h"

namespace SSSEngine
{
    template<typename T>
    struct FloatTraits;

    template<>
    struct FloatTraits<f32>
    {
        using Type = f32;
        using IntegerType = i32;
        using UnsignedIntegerType = u32;

        static constexpr u32 ExponentWidth = 8;
        static constexpr u32 MantissaWidth = 23;

        static constexpr UnsignedIntegerType ExponentMask = ((UnsignedIntegerType(1) << ExponentWidth) - 1) << MantissaWidth;
        static constexpr UnsignedIntegerType MantissaMask = ((UnsignedIntegerType(1) << MantissaWidth) - 1);

        /**
         * @brief The maximum number that can be represented by a 32-bit floating point number
         */
        static constexpr f32 Max = __FLT_MAX__;

        /**
         * @brief The closest number to 0 possible to represent with a 32-bit floating point number
         */
        static constexpr f32 SmallestPositive = __FLT_MIN__;

        /**
         * @brief The difference between 1.0f and the next number representable by a 32-bit floating point number
         */
        static constexpr f32 Epsilon = __FLT_EPSILON__;

        static constexpr f32 PositiveInfinity = BitCopy<f32>(ExponentMask);
        static constexpr f32 NegativeInfinity = -PositiveInfinity;
        static constexpr f32 NaN = BitCopy<f32>(Join(ExponentMask, MantissaMask));
        static constexpr f32 SignalingNaN = BitCopy<f32>(Join(ExponentMask, MantissaMask >> 1));
        static constexpr IntegerType BinaryDigits = MantissaWidth + 1;
    };

    template<>
    struct FloatTraits<f64>
    {
        using IntegerType = i64;
        using UnsignedIntegerType = u64;

        static constexpr u32 ExponentWidth = 11;
        static constexpr u32 MantissaWidth = 52;

        static constexpr UnsignedIntegerType ExponentMask = ((UnsignedIntegerType(1) << ExponentWidth) - 1) << MantissaWidth;
        static constexpr UnsignedIntegerType MantissaMask = ((UnsignedIntegerType(1) << MantissaWidth) - 1);

        /**
         * @brief The maximum number that can be represented by a 64-bit floating point number
         */
        static constexpr f64 Max = __DBL_MAX__;

        /**
         * @brief The closest number to 0 possible to represent with a 64-bit floating point number
         */
        static constexpr f64 SmallestPositive = __DBL_MIN__;

        /**
         * @brief The difference between 1.0f and the next number representable by a 64-bit floating point number
         */
        static constexpr f64 Epsilon = __DBL_EPSILON__;

        static constexpr f64 PositiveInfinity = BitCopy<f64>(ExponentMask);
        static constexpr f64 NegativeInfinity = -PositiveInfinity;
        static constexpr f64 NaN = BitCopy<f64>(Join(ExponentMask, MantissaMask));
        static constexpr f64 SignalingNaN = BitCopy<f64>(Join(ExponentMask, MantissaMask >> 1));
        static constexpr IntegerType BinaryDigits = MantissaWidth + 1;
    };

    template<FloatingPointConcept Float>
    using FloatToIntegerType = FloatTraits<Float>::Type;

    template<FloatingPointConcept Float>
    using FloatToIntegerUnsignedType = FloatTraits<Float>::UnsignedType;

    template<FloatingPointConcept Float>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto AsBits(Float value) noexcept
    {
        return BitCopy<FloatToIntegerUnsignedType<Float>>(value);
    }

    template<FloatingPointConcept Float>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto AsSignedBits(Float value) noexcept
    {
        return BitCopy<FloatToIntegerType<Float>>(value);
    }

    template<FloatingPointConcept Float>
    struct DecodedFloat
    {
        using Traits = FloatTraits<Float>;
        using UnsignedType = Traits::UnsignedIntegerType;

        UnsignedType sign : 1;
        UnsignedType exponent : Traits::ExponentWidth;
        UnsignedType mantissa : Traits::MantissaWidth;
    };

    SSSENGINE_STATIC_ASSERT(sizeof(f32) == sizeof(DecodedFloat<f32>) &&
                            (FloatTraits<f32>::ExponentWidth + FloatTraits<f32>::MantissaWidth == 31));
    SSSENGINE_STATIC_ASSERT(sizeof(f64) == sizeof(DecodedFloat<f64>) &&
                            (FloatTraits<f64>::ExponentWidth + FloatTraits<f64>::MantissaWidth == 63));

    template<FloatingPointConcept Float>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto SignBit(FloatToIntegerUnsignedType<Float> bits) noexcept
    {
        return bits >> (Bits<Float> - 1);
    }

    /**
     * @brief Checks the sign bit of the Floating point number
     *
     * @note Do not confuse with @seealso SignOf. This purely checks the value of the sign bit so there can be a -0 and
     * a +0, unlike the later.
     *
     * @param value The floating number to check
     * @return The value of the sign bit (1 or 0)
     */
    template<FloatingPointConcept Float>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto SignBit(Float value) noexcept
    {
        auto bits = AsBits(value);

        return SignBit(bits);
    }

    template<FloatingPointConcept Float>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto MaskExponent(FloatToIntegerUnsignedType<Float> bits) noexcept
    {
        return bits & FloatTraits<Float>::ExponentMask;
    }

    template<FloatingPointConcept Float>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto MaskExponent(Float value) noexcept
    {
        auto bits = AsBits(value);

        return MaskExponent<Float>(bits);
    }

    template<FloatingPointConcept Float>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto MaskMantissa(FloatToIntegerUnsignedType<Float> bits) noexcept
    {
        return bits & FloatTraits<Float>::MantissaMask;
    }

    template<FloatingPointConcept Float>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto MaskMantissa(Float value) noexcept
    {
        auto bits = AsBits(value);

        return MaskMantissa<Float>(bits);
    }

    template<FloatingPointConcept Float>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr DecodedFloat<Float> Decode(Float value) noexcept
    {
        auto bits = AsBits(value);

        return {.sign = SignBit<Float>(bits),
                .exponent = MaskExponent<Float>(bits) >> FloatTraits<Float>::MantissaWidth,
                .mantissa = MaskMantissa<Float>(bits)};
    }

    template<FloatingPointConcept R>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr bool IsNaN(R value) noexcept
    {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
        return __builtin_isnan(value);
#else
        auto bits = AsBits(value);

        auto exponent = MaskExponent<R>(bits);
        auto mantissa = MaskMantissa<R>(bits);

        return exponent == ExponentMask<R> && mantissa != 0;
#endif
    }

} // namespace SSSEngine
