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
 * @brief Utility to convert float from and to strings using the xjb algorithm
 */

#pragma once

#include "Array.h"
#include "AsciiEncoding.h"
#include "Attributes.h"
#include "Bits.h"
#include "Concepts.h"
#include "Debug.h"
#include "Endian.h"
#include "Float.h"
#include "HelperMacros.h"
#include "Architecture.h"
#include "Math.h"
#include "MemoryUtility.h"
#include "Simd.h"
#include "String.h"
#include "StringView.h"
#include "System.h"
#include "Types.h"
#include "Byte.h"
#include "Utf8Encoding.h"
#include "Byteswap.h"
#include <emmintrin.h>
#include <immintrin.h>

namespace SSSEngine::Text
{
    // =================================================================================================================
    // F32
    // =================================================================================================================

    namespace F32Lookup
    {
        // NOTE: Range between -32 and 44
        SSSENGINE_GLOBAL
        constexpr u64 Elements = 44 - (-32) + 1;
        SSSENGINE_GLOBAL
        constexpr auto ReversePowerOf10 = []
        {
            Containers::Array<u64, Elements> arr; // NOLINT(cppcoreguidelines-pro-type-member-init)

            u64 high = 0x67de18eda5814af3;
            u64 low = 0xcfb11ead453994ba;
            u64 ten = 0xA000000000000000;
            for(SizeType i = 0; i < Elements; ++i)
            {
                SizeType e10 = i - 32;
                arr[Elements - i - 1] = e10 == 0 ? u64(1) << 63 : low + 1;
                u64 h0 = (u128(high) * ten) >> 64;
                u64 c0 = h0 + low * ten;
                u64 c1 = (c0 < h0) + (u128(low) * ten >> 64);
                if(c1 >> 63)
                {
                    high = c0;
                    low = c1;
                }
                else
                {
                    high = c0 << 1;
                    low = Join(c1 << 1, c0 >> 63);
                }
            }
            return arr;
        }();

        SSSENGINE_GLOBAL
        constexpr auto H37 = []
        {
            Containers::Array<u8, 256> arr; // NOLINT(cppcoreguidelines-pro-type-member-init)
            for(int exp = 0; exp < 256; exp++)
            {
                int expBin = exp - 150 + (exp == 0);
                int k = (expBin * 1233) >> 12;
                int precalc = (36 + 1) + expBin + ((k * -1701 + (-1701)) >> 9);
                arr[exp] = (u8)precalc;
            }
            return arr;
        }();

#ifdef SSSENGINE_AARCH64
        SSSENGINE_GLOBAL
        constexpr auto C1 = (((u64)('0' + '0' * 256) << (36)) + (((u64)1 << (36 - 1)) - 7));
#else
        SSSENGINE_GLOBAL
        constexpr auto C1 = (((u64)('0' + '0' * 256) << (36 - 1)) + (((u64)1 << (36 - 2)) - 7));
#endif
        SSSENGINE_GLOBAL
        constexpr auto Div10000 = 1844674407370956;
        SSSENGINE_GLOBAL
        constexpr auto M = (1ULL << 32) - 10000;
        SSSENGINE_GLOBAL
        constexpr auto E7 = 10000000;
        SSSENGINE_GLOBAL
        constexpr auto E6 = 1000000;
        SSSENGINE_GLOBAL
        constexpr auto M32x4 = {0x147b000, -100 + 0x10000, 0xce0, -10 + 0x100};
    }; // namespace F32Lookup

    struct Decimal8ToAsciiResult
    {
        u64 ascii;
        u64 significantDigitsSub1;
    };

    constexpr Decimal8ToAsciiResult Decimal8ToAsciiAvx512(const u64 m, const u32 upDown, const u32 lz)
    {
        SSSENGINE_TODO;
    }

    SSSENGINE_CONST
    constexpr Decimal8ToAsciiResult Decimal8ToAsciiAvx2(const u64 m, const u32 upDown, const u32 lz)
    {
        u64 merge = (m << 32) + (1 - (u64(10'000) << 32)) * ((m * u128(1844674407370956)) >> 64);
        u64x2 y(merge);
        u32x4 a((100 << 16) - 1);
        u32x4 z = (u32x4(y) << 16) - (a * u32x4(MulHi(i16x8(y), i16x8(i32x4(10486))) >> 4));
        i16x8 zDiv10 = MulHi(i16x8(z), i16x8(0x199a));
        auto tmp = (i16x8(z) << 8) - (i16x8(2559) * zDiv10);
        u64 bcd = Lower64Bits(tmp);

        SSSENGINE_FUNCTION_LOCAL constexpr u64 Zero = 0x3030303030303030;
        u32 tz = Math::CountLeftZeros<true>(bcd) >> 3;
        bcd = bcd >> (lz << 3);
        bcd = System::IsLittleEndian() ? bcd : Byteswap(bcd);

        return {.ascii = Join(bcd, Zero), .significantDigitsSub1 = upDown ? (7 ^ lz) - tz : 8 - lz};
    }

    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr Decimal8ToAsciiResult Decimal8ToAscii(const u64 m, const u32 upDown, const u32 lz)
    {
#if defined(SSSENGINE_X64)
    #ifdef SSSENGINE_AVX512
        return Decimal8ToAsciiAvx512(m, upDown, lz);
    #elif defined(SSSENGINE_AVX2)
        return Decimal8ToAsciiAvx2(m, upDown, lz);
    #else
        if consteval
        {
            return Decimal8ToAsciiAvx2(m, upDown, lz);
        }
        else
        {
            // TODO: Check correct for avx512 implementations
            if(System::HasAvx512BW())
            {
                return Decimal8ToAsciiAvx512(m, upDown, lz);
            }
            return Decimal8ToAsciiAvx2(m, upDown, lz);
        }
    #endif
#elif defined(SSSENGINE_NEON)
        SSSENGINE_NOT_IMPLEMENTED
#endif
    }

    struct FloatToAsciiResult
    {
        Containers::Array<char, 24> digits;
        u32 significantDigits;
        i32 exponent;
    };

    /**
     * @brief Deconstructs a 32-bit float into it's digits and exponent
     *
     * @important: Do not pass the values 0, NaN or +-Infinity
     *
     * @param v The float to convert
     * @return The digits and exponent
     */
    SSSENGINE_PURE SSSENGINE_GLOBAL
    constexpr FloatToAsciiResult FloatToAscii(float v) noexcept
    {
        u32 bits = AsBits(v);

        u32 mantissa = Mantissa<f32>(bits);
        u64 exponent = Exponent<f32>(bits);

        u64 c = mantissa | (1 << FloatTraits<f32>::MantissaWidth);
        SSSENGINE_FUNCTION_LOCAL constexpr i64 ExponentBias = 127 + 23;
        i64 q = (i64)exponent - ExponentBias;

        if(exponent == 0) SSSENGINE_UNLIKELY
        {
            SSSENGINE_ASSERT(mantissa != 0);

            q = 1 - ExponentBias;
            c = mantissa;
        }

        SSSENGINE_ASSERT(exponent != 255);

#ifdef SSSENGINE_AARCH64 // for arm64 processor , fewer instructions
        i64 k = ((i64)biasedExponent * (u128)(1233ull << 52)) >> 64; // signed multiplication
#else
        i64 k = (q * 1233) >> 12;
#endif

        bool irregular = mantissa == 0;
        SSSENGINE_FUNCTION_LOCAL constexpr int Bit = 36;

        u8 h37 = F32Lookup::H37[exponent];
        if(irregular) SSSENGINE_UNLIKELY
        {
            k = (i64)(q * 1233 - 512) >> 12;
            h37 = (Bit + 1) + q + ((k * -1701 + (-1701)) >> 9);
        }

        u64 pow10 = F32Lookup::ReversePowerOf10[k + 45];
        u64 cb = c << h37;
        u64 hi64 = u128(cb) * pow10 >> 64;
        u64 halfUlp = (pow10 >> (65 - h37)) + ((mantissa + 1) & 1);
        u64 dotOne36Bit = hi64 & ((u64(1) << Bit) - 1);
        u32 mUp = (hi64 + halfUlp) >> Bit;
        u32 upDown = mUp > u32((hi64 - (halfUlp >> 0)) >> Bit);

#ifdef SSSENGINE_AARCH64
        u32 one = (dotOne36Bit * 10 + LookupTables::C1 + (dotOne36Bit >> (Bit - 4))) >> Bit;
#else
        u32 one = (dotOne36Bit * 5 + F32Lookup::C1 + (dotOne36Bit >> (Bit - 4))) >> (Bit - 1);
#endif
        if(irregular) SSSENGINE_UNLIKELY
        {
            if((q == 31 - 150) | (q == 214 - 150) | (q == 217 - 150))
            {
                ++one;
            }
            upDown = mUp > ((hi64 - (halfUlp >> 1)) >> Bit);
        }

        u32 lz = (u32(mUp) < u32(F32Lookup::E7)) + (u32(mUp) < u32(F32Lookup::E6));

        auto ascii = Decimal8ToAscii(mUp, upDown, lz);
        SizeType onePos = 8 - lz;
        i32 e10 = static_cast<i32>(k + (onePos));

        FloatToAsciiResult result{
            .digits{}, .significantDigits = static_cast<u32>(ascii.significantDigitsSub1 + 1), .exponent = e10};

        auto digits = BitCopy<Containers::Array<char, 8>>(ascii.ascii);
        auto oneDigits = BitCopy<Containers::Array<char, 4>>(one);

        RawMemoryCopy(digits.Data(), result.digits.Data(), 8);
        RawMemoryCopy(oneDigits.Data(), &result.digits[onePos], 2);

#ifdef SSSENGINE_AARCH64
        if(exp == 0) SSSENGINE_UNLIKELY
#endif
            if(mUp < 100'000) SSSENGINE_UNLIKELY
            {
                Containers::Array<char, 8> uArr; // NOLINT(cppcoreguidelines-pro-type-member-init)
                RawMemoryCopy(&result.digits[2], uArr.Data(), 8);
                u64 u = BitCopy<u64>(uArr);
                u = System::IsLittleEndian() ? u : Byteswap(u);
                i32 lz = Math::CountRightZeros(u & 0x0f0f0f0f0f0f0f0f) / 8;
                lz += 2;
                e10 -= lz;
                result.digits[0] = result.digits[lz];
                result.exponent = e10;
                result.significantDigits = 8 - lz - 1;
                RawMemoryMove(&result.digits[lz + 1], &result.digits[1], 8);
            }

        return result;
    }

    // =================================================================================================================
    // F64
    // =================================================================================================================

    namespace F64Lookup
    {
        SSSENGINE_GLOBAL
        constexpr int Offset = 9;
        SSSENGINE_GLOBAL
        constexpr int ExponentBias = 1075;
        SSSENGINE_GLOBAL
        constexpr SizeType Pow10Number = 323 - (-293) + 1;

        using PowerOf10Array = Containers::Array<u64, Pow10Number * 2>;
        SSSENGINE_GLOBAL
        constexpr PowerOf10Array PowerOf10 = []
        {
            PowerOf10Array arr; // NOLINT(cppcoreguidelines-pro-type-member-init)

            u64 w0{0xb2e28cedd086d011};
            u64 w1{0x1e53ed49a96272c8};
            u64 w2{0xcc5fc196fefd7d0c};
            SSSENGINE_FUNCTION_LOCAL constexpr u64 Ten = 0xa000000000000000;

            for(SizeType i = 0; i < Pow10Number; ++i)
            {
                int e10 = int(i) - 293;

                const SizeType index = (Pow10Number - 1 - i) * 2;
                arr[index] = e10 == 0 ? u64(1) << 63 : w2 + (e10 >= 0 && e10 <= 27);
                arr[index + 1] = w1 + 1;

                u64 h0 = (u128(w0) * Ten) >> 64;
                u64 h1 = (u128(w1) * Ten) >> 64;
                u64 c0 = h0 + w1 * Ten;
                u64 c1 = (c0 < h0) + h1 + w2 * Ten;
                u64 c2 = (c1 < h1) + ((u128(w2) * Ten) >> 64);
                if(c2 >> 63)
                {
                    w0 = c0;
                    w1 = c1;
                    w2 = c2;
                }
                else
                {
                    w0 = c0 << 1;
                    w1 = c1 << 1 | c0 >> 63;
                    w2 = c2 << 1 | c1 >> 63;
                }
            }
            return arr;
        }();

        SSSENGINE_GLOBAL
        constexpr auto H37 = []
        {
            SSSENGINE_FUNCTION_LOCAL constexpr int Size = 2048;
            Containers::Array<u8, Size> arr; // NOLINT(cppcoreguidelines-pro-type-member-init)
            for(int exp = 0; exp < Size; exp++)
            {
                int q = exp - ExponentBias + (exp == 0);
                int k = (q * 78913) >> 18;
                int h = q + (((-k - 1) * 217707) >> 16);
                arr[exp] = u8(h + 1 + Offset);
            }
            return arr;
        }();

        SSSENGINE_GLOBAL
        constexpr u64 C1 = 78913ull << (64 - 18);
        SSSENGINE_GLOBAL
        constexpr u64 C2 = (u64)-217707;
        SSSENGINE_GLOBAL
        constexpr u64 C3 = (u64)1e15 - 1;
        SSSENGINE_GLOBAL
        constexpr u64 C4 = (1ULL << 63) + 6;
        SSSENGINE_GLOBAL
        constexpr u64 C5 = (u64)-131072;
        SSSENGINE_GLOBAL
        constexpr u64 C6 = (1 << 9) - 1;
        SSSENGINE_GLOBAL
        constexpr u64 MulConst = 0xabcc77118461cefd;
    } // namespace F64Lookup

    struct Ascii16
    {
        u8x16 ascii;
        u64 significantDigitsSub1;
    };

    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr Ascii16 DecimalToAscii16Avx2(u64 m, u64 upDown, u64 d17, u32 abcdefgh, u32 ijklmnop) noexcept
    {
        auto x = UnpackLow(u64x2(CopyInt(ijklmnop)), u64x2(CopyInt(abcdefgh)));
        SSSENGINE_FUNCTION_LOCAL constexpr auto Lhs = u64x2((u64(1) << 32) - 10'000);
        auto rhs = MultiplyLow32Bit(u64x2(x), u64x2(109951163));
        auto y = u64x2(x) + u64x2(u32x4(MultiplyLow32Bit(Lhs, rhs >> 40)));

        auto z = u64x2(y) + u64x2(u32x4((1 << 16) - 100) * (u32x4(MulHi(u16x8(y), u16x8(0x147b))) >> 3));

        auto bcdSwapped = u16x8(z) + u16x8((1 << 8) - 10) * MulHi(u16x8(z), u16x8(0x199a));
        SSSENGINE_FUNCTION_LOCAL constexpr u8x16 Zero('0');
        auto ascii16Swapped = u8x16(bcdSwapped) + Zero;
        auto ascii16 = Reverse(ascii16Swapped);
        int mask = MoveMask(u8x16(u8x16(bcdSwapped) > u8x16(0)));

        int tz = Math::CountRightZeros(mask);
        auto digits = upDown ? 14 + d17 - tz : 15 + d17;

        return {.ascii = ascii16, .significantDigitsSub1 = static_cast<u64>(digits)};
    }

    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr Ascii16 DecimalToAscii16(u64 m, u64 upDown, u64 d17) noexcept
    {
        u32 abcdefgh = (u128(m) * F64Lookup::MulConst) >> (90);

        SSSENGINE_FUNCTION_LOCAL constexpr i64 HundredMillion = -100'000'000;
        u32 ijklmnop = m + abcdefgh * HundredMillion;

#ifdef SSSENGINE_X64
    #ifdef SSSENGINE_AVX512
    #elif defined(SSSENGINE_AVX2)
    #else
        if consteval
        {
            return DecimalToAscii16Avx2(m, upDown, d17, abcdefgh, ijklmnop);
        }
        else
        {
            if(System::HasAvx512BW())
            {
                SSSENGINE_TODO;
            }
            else if(System::HasAvx2())
            {
                return DecimalToAscii16Avx2(m, upDown, d17, abcdefgh, ijklmnop);
            }
        }
    #endif
#elif defined(SSSENGINE_NEON)
        SSSENGINE_NOT_IMPLEMENTED;
#endif
    }

    /**
     * @brief Deconstructs a 64-bit float into it's digits and exponent
     *
     * @important: Do not pass the values 0, NaN or +-Infinity
     *
     * @param v The double to convert
     * @return The digits and exponent
     */
    SSSENGINE_PURE SSSENGINE_GLOBAL
    constexpr FloatToAsciiResult FloatToAscii(double v) noexcept

    {
        u64 vi = AsBits(v);
        u64 mantissa = Mantissa<f64>(vi);
        u64 exponent = Exponent<f64>(vi);

        u64 c = mantissa | (u64(1) << FloatTraits<f64>::MantissaWidth);
        SSSENGINE_FUNCTION_LOCAL constexpr i64 ExponentBias = 1075;
        i64 q = (i64)exponent - ExponentBias;

#ifdef SSSENGINE_X64
        if(((exponent + 1) & 2047) <= 1) SSSENGINE_UNLIKELY
#endif // SSSENGINE_X64
        {
            if(exponent == 0) SSSENGINE_UNLIKELY
            {
                SSSENGINE_ASSERT(mantissa != 0);

                if(mantissa == 1)
                {
                    return {.digits{'5'}, .significantDigits = 1, .exponent = -324};
                }

                q = 1 - ExponentBias;
                c = mantissa;
            }

            SSSENGINE_ASSERT(exponent != 2047);
        }

        auto h7Precalc = F64Lookup::H37[exponent];
        bool irregular = mantissa == 0;

#ifdef SSSENGINE_AARCH64
        i64 k = ((i64(exponent) - ExponentBias) * u128(u64(78913) << (64 - 18))) >> 64;
#else
        i64 k = ((i64(exponent) - ExponentBias) * 78913) >> 18;
#endif // SSSENGINE_AARCH64
        const u64 *pow10 = F64Lookup::PowerOf10.Data() + 648 + k * 2;
        u64 powerHi = pow10[0];
        u64 powerLow = pow10[1];
        // TODO: Can we just use u128?
        auto b = c << h7Precalc;
        u128 result = (u128(b) * powerHi + ((u128(b) * powerLow) >> 64));
        u64 resultHi = result >> 64;
        u64 resultLow = u64(result);
        u64 dotOne = (resultHi << (64 - F64Lookup::Offset)) | (resultLow >> F64Lookup::Offset);
        u64 halfUlp = (powerHi >> ((1 + F64Lookup::Offset) - h7Precalc)) + ((c + 1) & 1);
        bool up = halfUlp > ~0 - dotOne;
        bool down = halfUlp > dotOne;
        u64 mUp = (resultHi >> F64Lookup::Offset) + up;
        u32 upDown = up + down;
        u64 half = (dotOne == (u64(1) << 62)) ? 0 : F64Lookup::C4;
        u32 one = (u128(dotOne) * 10 + half) >> 64;

        if(irregular) SSSENGINE_UNLIKELY
        {
            k = i64(q * 315653 - 131072) >> 20;
            i64 h = q + ((k * -217707 - 217707) >> 16);
            u64 powerHi = F64Lookup::PowerOf10[323 * 2 + 2 + k * 2];
            u64 halfUlp = powerHi >> -h;
            u64 dotOne = powerHi << (53 + h);
            u64 up = halfUlp > ~0 - dotOne;
            u64 down = (halfUlp >> 1) > dotOne;
            mUp = (powerHi >> (11 - h)) + up;
            upDown = up + down;
            one = ((dotOne >> (53 + h)) * 5 + (1 << (9 - h))) >> (10 - h);
            if((((dotOne >> 54) * 5) & ((1 << 9) - 1)) > ((halfUlp >> 55) * 5))
            {
                one = (((dotOne >> 54) * 5) >> 9) + 1;
            }
            if(dotOne == (u64(1) << 62))
            {
                one = 2;
            }
        }

        u64 d17 = mUp > F64Lookup::C3;
        u64 mr = d17 ? mUp : mUp * 10;
        i32 e10 = i32(k + 15 + d17);

        auto ascii = DecimalToAscii16(mr, upDown, d17);

        FloatToAsciiResult final{
            .digits{}, .significantDigits = static_cast<u32>(ascii.significantDigitsSub1 + 1), .exponent = e10};

        if consteval
        {
            // NOTE: This is a hack to work in compile time since we don't use char as an Integral, we can't use store
            // with it. Reinterpret cast is disallowed under constant expressions so we must do conversions allowed.
            Containers::Array<u8, 16> digits; // NOLINT(cppcoreguidelines-pro-type-member-init)
            Store(ascii.ascii, digits.Data());

            auto digitsChar = BitCopy<Containers::Array<char, 16>>(digits);
            RawMemoryCopy(digitsChar.Data(), final.digits.Data(), 16);
        }
        else
        {
            Store(ascii.ascii, reinterpret_cast<u8 *>(final.digits.Data()));
        }

#ifdef SSSENGINE_AARCH64
        if(exponent == 0) SSSENGINE_UNLIKELY
#endif
            if(mUp < u64(1e14)) SSSENGINE_UNLIKELY
            {
                SSSENGINE_TODO;
            }

        return final;
    }

    constexpr auto A = FloatToAscii(198239.298239);
    constexpr auto B = A.exponent;
    constexpr auto C = A.digits;
    constexpr auto C0 = C[0];
    constexpr auto C1 = C[1];
    constexpr auto C2 = C[2];
    constexpr auto C3 = C[3];
    constexpr auto C4 = C[4];
    constexpr auto C5 = C[5];
    constexpr auto C6 = C[6];
    constexpr auto C7 = C[7];
    constexpr auto C8 = C[8];
    constexpr auto C9 = C[9];
    constexpr auto C10 = C[10];
    constexpr auto C11 = C[11];
    constexpr auto C12 = C[12];
    constexpr auto C13 = C[13];
    constexpr auto C14 = C[14];
    constexpr auto C15 = C[15];
    constexpr auto D = A.significantDigits;

} // namespace SSSEngine::Text
