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

#include "Architecture.h"

#include "Attributes.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "Math.h"
#include "System.h"
#include "Types.h"

#include <immintrin.h>

#define SSSENGINE_SIMD_ATTRIBUTES SSSENGINE_CONST SSSENGINE_FORCE_INLINE

// NOLINTBEGIN(portability-simd-intrinsics)
namespace SSSEngine
{
#ifdef SSSENGINE_X64

    template<u32 VectorSize>
    SSSENGINE_GLOBAL
    constexpr u32 MaxLanes = VectorSize / 8;

    template<u32 Lanes, u32 VectorSize>
    SSSENGINE_GLOBAL
    constexpr bool ValidLanesNumber = Lanes <= MaxLanes<VectorSize> && Math::IsPowerOf2(Lanes);

    template<SizeType>
    struct IntBySize;

    template<>
    struct IntBySize<64>
    {
        using Type = i64;
    };

    template<>
    struct IntBySize<32>
    {
        using Type = i32;
    };

    template<>
    struct IntBySize<16>
    {
        using Type = i16;
    };

    template<>
    struct IntBySize<8>
    {
        using Type = i8;
    };

    template<u32 Lanes>
        requires(Lanes >= 2) && ValidLanesNumber<Lanes, 128>
    struct Int128
    {
        static constexpr SizeType ElementSize = 128 / Lanes;

        using NativeType = __m128i;
        using ElementType = IntBySize<ElementSize>::Type;

        Int128() = default;

        // NOLINTBEGIN(google-explicit-constructor)

        SSSENGINE_FORCE_INLINE
        constexpr Int128(NativeType value) noexcept :
            value(value)
        {
        }

        // NOLINTEND(google-explicit-constructor)

        SSSENGINE_FORCE_INLINE
        constexpr explicit Int128(ElementType value) noexcept :
            value(NativeSet(value))
        {
        }

        template<u32 L>
        SSSENGINE_FORCE_INLINE
        constexpr explicit Int128(Int128<L> other) noexcept :
            value{other.value}
        {
        }

        SSSENGINE_SIMD_ATTRIBUTES
        static consteval Int128 Zero()
        {
            return _mm_setzero_si128();
        }

        // ---------
        // Operators
        // ---------

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator+(Int128 lhs, Int128 rhs) noexcept
        {
            return NativeAdd(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator-(Int128 lhs, Int128 rhs) noexcept
        {
            return NativeSub(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator*(Int128 lhs, Int128 rhs) noexcept
        {
            return NativeMultiply(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator&(Int128 lhs, Int128 rhs) noexcept
        {
            return NativeAnd(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator|(Int128 lhs, Int128 rhs) noexcept
        {
            return NativeOr(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator^(Int128 lhs, Int128 rhs) noexcept
        {
            return NativeXor(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator<<(Int128 lhs, Int128 rhs) noexcept
        {
            return NativeLeftShiftIndependent(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator<<(Int128 lhs, int count) noexcept
        {
            return NativeLeftShift(lhs.value, count);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator>>(Int128 lhs, Int128 rhs) noexcept
        {
            return NativeRightShiftIndependent(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator>>(Int128 lhs, int count) noexcept
        {
            return NativeRightShift(lhs.value, count);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator==(Int128 lhs, Int128 rhs) noexcept
        {
            return NativeEquals(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator!=(Int128 lhs, Int128 rhs) noexcept
        {
            return ~(lhs == rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator>(Int128 lhs, Int128 rhs) noexcept
        {
            return NativeGreaterThan(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator<(Int128 lhs, Int128 rhs) noexcept
        {
            return NativeGreaterThan(rhs.value, lhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator>=(Int128 lhs, Int128 rhs) noexcept
        {
            return ~(lhs > rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int128 operator<=(Int128 lhs, Int128 rhs) noexcept
        {
            return ~(lhs < rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator+=(Int128 other) noexcept
        {
            value = NativeAdd(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator-=(Int128 other) noexcept
        {
            value = NativeSub(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator*=(Int128 other) noexcept
        {
            value = NativeMultiply(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator&=(Int128 other) noexcept
        {
            value = NativeAnd(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator|=(Int128 other) noexcept
        {
            value = NativeOr(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator^=(Int128 other) noexcept
        {
            value = NativeXor(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator<<=(Int128 other) noexcept
        {
            value = NativeLeftShift(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator<<=(int count) noexcept
        {
            value = NativeLeftShift(value, count);

            return *this;
        }

        template<u32 L>
        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator<<=(Int128<L> other) noexcept
        {
            value = NativeLeftShift(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator>>=(Int128 other) noexcept
        {
            value = NativeRightShift(value, other.value);

            return *this;
        }

        template<u32 L>
            SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator>>=(Int128<L> other) noexcept
        {
            value = NativeRightShift(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator>>=(int count) noexcept
        {
            value = NativeRightShift(value, count);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator~() noexcept
        {
            SSSENGINE_FUNCTION_LOCAL constexpr NativeType Neg1 = NativeSet(-1);
            value = NativeXor(value, Neg1);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int128 &operator-() noexcept
        {
            SSSENGINE_FUNCTION_LOCAL constexpr NativeType Z = Zero().value;
            value = NativeSub(Z, value);

            return *this;
        }

        template<u32 L>
            requires ValidLanesNumber<L, 128>
        SSSENGINE_SIMD_ATTRIBUTES
        constexpr explicit operator Int128<L>() noexcept
        {
            return Int128<L>{value};
        }

        NativeType value;

      private:
        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeSet(ElementType value)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm128_set1_epi64x(value);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm128_set1_epi32(value);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm128_set1_epi16(value);
            }
            else
            {
                return _mm128_set1_epi8(value);
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeAdd(NativeType lhs, NativeType rhs)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm_add_epi64(lhs, rhs);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_add_epi32(lhs, rhs);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm_add_epi16(lhs, rhs);
            }
            else
            {
                return _mm_add_epi8(lhs, rhs);
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeSub(NativeType lhs, NativeType rhs)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm_sub_epi64(lhs, rhs);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_sub_epi32(lhs, rhs);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm_sub_epi16(lhs, rhs);
            }
            else
            {
                return _mm_sub_epi8(lhs, rhs);
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeMultiply(NativeType lhs, NativeType rhs)
        {
            if constexpr(ElementSize == 64)
            {
                SSSENGINE_ASSERT(System::HasAvx512DQ() && System::HasAvx512VL());
                return _mm_mullo_epi64(lhs, rhs);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_mullo_epi32(lhs, rhs);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm_mullo_epi16(lhs, rhs);
            }
            else
            {
                SSSENGINE_NOT_IMPLEMENTED;
                // INVESTIGATE: Should we emulate with a multiplication using i16?
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeAnd(NativeType lhs, NativeType rhs)
        {
            return _mm_and_si128(lhs, rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeOr(NativeType lhs, NativeType rhs)
        {
            return _mm_or_si128(lhs, rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeXor(NativeType lhs, NativeType rhs)
        {
            return _mm_xor_si128(lhs, rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeLeftShift(NativeType lhs, int count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm_slli_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_slli_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm_slli_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No left shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeLeftShift(NativeType lhs, NativeType count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm_sll_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_sll_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm_sll_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No left shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeLeftShiftIndependent(NativeType lhs, NativeType count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm_sllv_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_sllv_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                SSSENGINE_ASSERT(System::HasAvx512BW() && System::HasAvx512VL());
                return _mm_sllv_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No left shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeRightShift(NativeType lhs, int count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm_srli_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_srli_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm_srli_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No right shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeRightShift(NativeType lhs, __m128i count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm_srl_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_srl_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm_srl_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No right shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeRightShiftIndependent(NativeType lhs, NativeType count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm_srlv_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_srlv_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                SSSENGINE_ASSERT(System::HasAvx512BW() && System::HasAvx512VL());
                return _mm_srlv_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No right shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeEquals(NativeType lhs, NativeType rhs)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm_cmpeq_epi64(lhs, rhs);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_cmpeq_epi32(lhs, rhs);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm_cmpeq_epi16(lhs, rhs);
            }
            else
            {
                return _mm_cmpeq_epi8(lhs, rhs);
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeGreaterThan(NativeType lhs, NativeType rhs)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm_cmpgt_epi64(lhs, rhs);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_cmpgt_epi32(lhs, rhs);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm_cmpgt_epi16(lhs, rhs);
            }
            else
            {
                return _mm_cmpgt_epi8(lhs, rhs);
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeLessThan(NativeType lhs, NativeType rhs)
        {
            if constexpr(ElementSize == 64)
            {
                return NativeGreaterThan(rhs, lhs);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm_cmplt_epi32(lhs, rhs);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm_cmplt_epi16(lhs, rhs);
            }
            else
            {
                return _mm_cmplt_epi8(lhs, rhs);
            }
        }
    };

    using i64x2 = Int128<2>;
    using i32x4 = Int128<4>;
    using i16x8 = Int128<8>;
    using i8x16 = Int128<16>;

    template<u32 Lanes>
        requires(Lanes >= 4) && ValidLanesNumber<Lanes, 256>
    struct Int256
    {
        static constexpr SizeType ElementSize = 256 / Lanes;

        using NativeType = __m256i;
        using ElementType = IntBySize<ElementSize>::Type;
        static constexpr SizeType ElementCount = Lanes;

        Int256() = default;

        // NOLINTBEGIN(google-explicit-constructor)

        SSSENGINE_FORCE_INLINE
        constexpr Int256(NativeType value) noexcept :
            value(value)
        {
        }

        // NOLINTEND(google-explicit-constructor)

        SSSENGINE_FORCE_INLINE
        constexpr explicit Int256(ElementType value) noexcept :
            value(NativeSet(value))
        {
        }

        template<u32 L>
        SSSENGINE_FORCE_INLINE
        constexpr explicit Int256(Int256<L> other) noexcept :
            value{other.value}
        {
        }

        SSSENGINE_SIMD_ATTRIBUTES
        static consteval Int256 Zero()
        {
            return _mm256_setzero_si256();
        }

        // ---------
        // Operators
        // ---------

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator+(Int256 lhs, Int256 rhs) noexcept
        {
            return NativeAdd(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator-(Int256 lhs, Int256 rhs) noexcept
        {
            return NativeSub(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator*(Int256 lhs, Int256 rhs) noexcept
        {
            return NativeMultiply(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator&(Int256 lhs, Int256 rhs) noexcept
        {
            return NativeAnd(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator|(Int256 lhs, Int256 rhs) noexcept
        {
            return NativeOr(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator^(Int256 lhs, Int256 rhs) noexcept
        {
            return NativeXor(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator<<(Int256 lhs, Int256 rhs) noexcept
        {
            return NativeLeftShift(lhs.value, rhs.value);
        }

        template<u32 L>
        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator<<(Int256 lhs, Int128<L> count) noexcept
        {
            return NativeLeftShift(lhs.value, count.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator<<(Int256 lhs, int count) noexcept
        {
            return NativeLeftShift(lhs.value, count);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator>>(Int256 lhs, Int256 rhs) noexcept
        {
            return NativeRightShift(lhs.value, rhs.value);
        }

        template<u32 L>
        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator>>(Int256 lhs, Int128<L> count) noexcept
        {
            return NativeRightShift(lhs.value, count.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator>>(Int256 lhs, int count) noexcept
        {
            return NativeRightShift(lhs.value, count);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator==(Int256 lhs, Int256 rhs) noexcept
        {
            return NativeEquals(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator!=(Int256 lhs, Int256 rhs) noexcept
        {
            return ~(lhs == rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator>(Int256 lhs, Int256 rhs) noexcept
        {
            return NativeGreaterThan(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator<(Int256 lhs, Int256 rhs) noexcept
        {
            return NativeGreaterThan(rhs.value, lhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator>=(Int256 lhs, Int256 rhs) noexcept
        {
            return ~(lhs > rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Int256 operator<=(Int256 lhs, Int256 rhs) noexcept
        {
            return ~(lhs < rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator+=(Int256 other) noexcept
        {
            value = NativeAdd(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator-=(Int256 other) noexcept
        {
            value = NativeSub(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator*=(Int256 other) noexcept
        {
            value = NativeMultiply(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator&=(Int256 other) noexcept
        {
            value = NativeAnd(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator|=(Int256 other) noexcept
        {
            value = NativeOr(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator^=(Int256 other) noexcept
        {
            value = NativeXor(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator<<=(Int256 other) noexcept
        {
            value = NativeLeftShift(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator<<=(int count) noexcept
        {
            value = NativeLeftShift(value, count);

            return *this;
        }

        template<u32 L>
        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator<<=(Int128<L> other) noexcept
        {
            value = NativeLeftShift(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator>>=(Int256 other) noexcept
        {
            value = NativeRightShift(value, other.value);

            return *this;
        }

        template<u32 L>
            SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator>>=(Int128<L> other) noexcept
        {
            value = NativeRightShift(value, other.value);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator>>=(int count) noexcept
        {
            value = NativeRightShift(value, count);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator~() noexcept
        {
            SSSENGINE_FUNCTION_LOCAL constexpr NativeType Neg1 = NativeSet(-1);
            value = NativeXor(value, Neg1);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Int256 &operator-() noexcept
        {
            SSSENGINE_FUNCTION_LOCAL constexpr NativeType Z = Zero().value;
            value = NativeSub(Z, value);

            return *this;
        }

        template<u32 L>
            requires ValidLanesNumber<L, 256>
        SSSENGINE_SIMD_ATTRIBUTES
        constexpr explicit operator Int256<L>() noexcept
        {
            return Int256<L>{value};
        }

        NativeType value;

      private:
        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeSet(ElementType value)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm256_set1_epi64x(value);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_set1_epi32(value);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_set1_epi16(value);
            }
            else
            {
                return _mm256_set1_epi8(value);
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeAdd(NativeType lhs, NativeType rhs)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm256_add_epi64(lhs, rhs);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_add_epi32(lhs, rhs);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_add_epi16(lhs, rhs);
            }
            else
            {
                return _mm256_add_epi8(lhs, rhs);
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeSub(NativeType lhs, NativeType rhs)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm256_sub_epi64(lhs, rhs);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_sub_epi32(lhs, rhs);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_sub_epi16(lhs, rhs);
            }
            else
            {
                return _mm256_sub_epi8(lhs, rhs);
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeMultiply(NativeType lhs, NativeType rhs)
        {
            if constexpr(ElementSize == 64)
            {
                SSSENGINE_ASSERT(System::HasAvx512DQ() && System::HasAvx512VL());
                return _mm256_mullo_epi64(lhs, rhs);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_mullo_epi32(lhs, rhs);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_mullo_epi16(lhs, rhs);
            }
            else
            {
                SSSENGINE_NOT_IMPLEMENTED;
                // INVESTIGATE: Should we emulate with a multiplication using i16?
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeAnd(NativeType lhs, NativeType rhs)
        {
            return _mm256_and_si256(lhs, rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeOr(NativeType lhs, NativeType rhs)
        {
            return _mm256_or_si256(lhs, rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeXor(NativeType lhs, NativeType rhs)
        {
            return _mm256_xor_si256(lhs, rhs);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeLeftShift(NativeType lhs, int count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm256_slli_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_slli_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_slli_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No left shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeLeftShift(NativeType lhs, __m128i count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm256_sll_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_sll_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_sll_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No left shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeLeftShift(NativeType lhs, NativeType count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm256_sllv_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_sllv_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_sllv_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No left shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeRightShift(NativeType lhs, int count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm256_srli_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_srli_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_srli_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No right shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeRightShift(NativeType lhs, __m128i count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm256_srl_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_srl_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_srl_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No right shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeRightShift(NativeType lhs, NativeType count)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm256_srlv_epi64(lhs, count);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_srlv_epi32(lhs, count);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_srlv_epi16(lhs, count);
            }
            else
            {
                SSSENGINE_STATIC_ASSERT(false, "No right shift for 8bit values");
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeEquals(NativeType lhs, NativeType rhs)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm256_cmpeq_epi64(lhs, rhs);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_cmpeq_epi32(lhs, rhs);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_cmpeq_epi16(lhs, rhs);
            }
            else
            {
                return _mm256_cmpeq_epi8(lhs, rhs);
            }
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr static NativeType NativeGreaterThan(NativeType lhs, NativeType rhs)
        {
            if constexpr(ElementSize == 64)
            {
                return _mm256_cmpgt_epi64(lhs, rhs);
            }
            else if constexpr(ElementSize == 32)
            {
                return _mm256_cmpgt_epi32(lhs, rhs);
            }
            else if constexpr(ElementSize == 16)
            {
                return _mm256_cmpgt_epi16(lhs, rhs);
            }
            else
            {
                return _mm256_cmpgt_epi8(lhs, rhs);
            }
        }
    };

    using i64x4 = Int256<4>;
    using i32x8 = Int256<8>;
    using i16x16 = Int256<16>;
    using i8x32 = Int256<32>;

    template<u32 Lanes>
    struct alignas(64) Int512
    {
    #if defined(SSSENGINE_SIMD_AVX2)
            // TODO: Fallback to using AVX2
    #else
            //  TODO: Native AVX512
    #endif
    };

    using i64x8 = Int512<8>;
    using i32x16 = Int512<16>;
    using i16x32 = Int512<32>;
    using i8x64 = Int512<64>;

    struct Float256
    {
        using NativeType = __m256;
        using ElementType = f32;
        static constexpr SizeType ElementCount = 8;

        Float256() = default;

        // NOLINTBEGIN(google-explicit-constructor)

        SSSENGINE_FORCE_INLINE
        constexpr Float256(NativeType value) noexcept :
            value(value)
        {
        }

        // NOLINTEND(google-explicit-constructor)

        SSSENGINE_FORCE_INLINE
        constexpr explicit Float256(ElementType value) noexcept :
            value(_mm256_set1_ps(value))
        {
        }

        // ---------
        // Operators
        // ---------

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator+(Float256 lhs, Float256 rhs)
        {
            return _mm256_add_ps(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator-(Float256 lhs, Float256 rhs)
        {
            return _mm256_sub_ps(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator*(Float256 lhs, Float256 rhs)
        {
            return _mm256_mul_ps(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator/(Float256 lhs, Float256 rhs)
        {
            return _mm256_div_ps(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator&(Float256 lhs, Float256 rhs)
        {
            return _mm256_and_ps(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator|(Float256 lhs, Float256 rhs)
        {
            return _mm256_or_ps(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator^(Float256 lhs, Float256 rhs)
        {
            return _mm256_xor_ps(lhs.value, rhs.value);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator==(Float256 lhs, Float256 rhs)
        {
            return _mm256_cmp_ps(lhs.value, rhs.value, _CMP_EQ_OQ);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator!=(Float256 lhs, Float256 rhs)
        {
            return _mm256_cmp_ps(lhs.value, rhs.value, _CMP_NEQ_OQ);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator>(Float256 lhs, Float256 rhs)
        {
            return _mm256_cmp_ps(lhs.value, rhs.value, _CMP_GT_OQ);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator>=(Float256 lhs, Float256 rhs)
        {
            return _mm256_cmp_ps(lhs.value, rhs.value, _CMP_GE_OQ);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator<(Float256 lhs, Float256 rhs)
        {
            return _mm256_cmp_ps(lhs.value, rhs.value, _CMP_LT_OQ);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr friend Float256 operator<=(Float256 lhs, Float256 rhs)
        {
            return _mm256_cmp_ps(lhs.value, rhs.value, _CMP_LE_OQ);
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Float256 &operator+=(Float256 other) noexcept
        {
            value = (*this + other).value;

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Float256 &operator-=(Float256 other) noexcept
        {
            value = (*this - other).value;

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Float256 &operator*=(Float256 other) noexcept
        {
            value = (*this * other).value;

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Float256 &operator/=(Float256 other) noexcept
        {
            value = (*this / other).value;

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Float256 &operator&=(Float256 other) noexcept
        {
            value = (*this & other).value;

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Float256 &operator|=(Float256 other) noexcept
        {
            value = (*this | other).value;

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Float256 &operator^=(Float256 other) noexcept
        {
            value = (*this ^ other).value;

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Float256 &operator~() noexcept
        {
            SSSENGINE_FUNCTION_LOCAL constexpr NativeType Neg1 = _mm256_castsi256_ps(_mm256_set1_epi64x(-1.0f));

            value = _mm256_xor_ps(value, Neg1);

            return *this;
        }

        SSSENGINE_SIMD_ATTRIBUTES
        constexpr Float256 &operator-() noexcept
        {
            SSSENGINE_FUNCTION_LOCAL constexpr NativeType Neg0 = _mm256_set1_ps(-0.0f);

            value = _mm256_xor_ps(value, Neg0);

            return *this;
        }

        NativeType value;
    };

    Float256 Fma(Float256 a, Float256 b, Float256 c)
    {
        SSSENGINE_ASSERT(System::HasFma());
        return _mm256_fmadd_ps(a.value, b.value, c.value);
    }

#endif
} // namespace SSSEngine

// NOLINTEND(portability-simd-intrinsics)
