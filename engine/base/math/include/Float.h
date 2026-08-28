/**
 * @file
 * @brief
 */

#pragma once

#include "Attributes.h"
#include "Concepts.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "Limits.h"
#include "Bits.h"
#include "Types.h"

namespace SSSEngine::Math
{
    template<FloatingPointConcept Float>
    struct FloatToIntegerTypeConverter
    {
    };

    template<>
    struct FloatToIntegerTypeConverter<f32>
    {
        using Type = i32;
        using UnsignedType = u32;
    };

    template<>
    struct FloatToIntegerTypeConverter<f64>
    {
        using Type = i64;
        using UnsignedType = u64;
    };

    template<FloatingPointConcept Float>
    using FloatToIntegerType = FloatToIntegerTypeConverter<Float>::Type;

    template<FloatingPointConcept Float>
    using FloatToIntegerUnsignedType = FloatToIntegerTypeConverter<Float>::UnsignedType;

    template<FloatingPointConcept R>
    SSSENGINE_GLOBAL
    constexpr u64 ExponentWidth = 0;

    template<FloatingPointConcept R>
    SSSENGINE_GLOBAL
    constexpr u64 MantissaWidth = 0;

    template<>
    SSSENGINE_GLOBAL
    constexpr u32 ExponentWidth<f32> = 8;

    template<>
    SSSENGINE_GLOBAL
    constexpr u32 MantissaWidth<f32> = 23;

    template<>
    SSSENGINE_GLOBAL
    constexpr u64 ExponentWidth<f64> = 11;

    template<>
    SSSENGINE_GLOBAL
    constexpr u64 MantissaWidth<f64> = 52;

    template<FloatingPointConcept R>
    SSSENGINE_GLOBAL
    constexpr auto ExponentMask = ((FloatToIntegerUnsignedType<R>(1) << ExponentWidth<R>)-1) << MantissaWidth<R>;

    template<FloatingPointConcept R>
    SSSENGINE_GLOBAL
    constexpr auto MantissaMask = ((FloatToIntegerUnsignedType<R>(1) << MantissaWidth<R>)-1);

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
    };

    template<>
    struct DecodedFloat<f32>
    {
        u32 sign : 1;
        u32 exponent : ExponentWidth<f32>;
        u32 mantissa : MantissaWidth<f32>;
    };

    template<>
    struct DecodedFloat<f64>
    {
        u64 sign : 1;
        u64 exponent : ExponentWidth<f64>;
        u64 mantissa : MantissaWidth<f64>;
    };

    SSSENGINE_STATIC_ASSERT(sizeof(f32) == sizeof(DecodedFloat<f32>) && (ExponentWidth<f32> + MantissaWidth<f32> == 31));
    SSSENGINE_STATIC_ASSERT(sizeof(f64) == sizeof(DecodedFloat<f64>) && (ExponentWidth<f64> + MantissaWidth<f64> == 63));

    template<FloatingPointConcept Float>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto SignBit(FloatToIntegerUnsignedType<Float> bits) noexcept
    {
        return bits >> (Limits::Bits<Float> - 1);
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
        return bits & ExponentMask<Float>;
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
        return bits & MantissaMask<Float>;
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
                .exponent = MaskExponent<Float>(bits) >> MantissaWidth<Float>,
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

    template<FloatingPointConcept Float>
    SSSENGINE_GLOBAL
    constexpr Float PositiveInfinity = BitCopy<Float>(ExponentMask<Float>);

    template<FloatingPointConcept Float>
    SSSENGINE_GLOBAL
    constexpr Float NegativeInfinity = -PositiveInfinity<Float>;

    template<FloatingPointConcept Float>
    SSSENGINE_GLOBAL
    constexpr Float NaN = BitCopy<Float>(Join(ExponentMask<Float>, MantissaMask<Float>));

    template<FloatingPointConcept Float>
    SSSENGINE_GLOBAL
    constexpr Float SignalingNaN = BitCopy<Float>(Join(ExponentMask<Float>, MantissaMask<Float> >> 1));

} // namespace SSSEngine::Math
