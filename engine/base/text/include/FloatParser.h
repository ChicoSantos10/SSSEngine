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

#if defined(SSSENGINE_X64)
// On x86-64 processors, memmove can trigger store-to-load forwarding stalls.
    #define SSSENGINE_NO_MEMMOVE 1
#elif defined(SSSENGINE_NEON) && defined(SSSENGINE_APPLE) && defined(SSSENGINE_AARCH64)
// On Apple Silicon processors, memmove is a performance gain.
    #define SSSENGINE_NO_MEMMOVE 0
#elif defined(SSSENGINE_NEON)
// On other aarch64 implementations the performance characteristics of memmove may differ.
    #define SSSENGINE_NO_MEMMOVE 1
#else
    #define SSSENGINE_NO_MEMMOVE 0
#endif

namespace SSSEngine::Text
{
    struct DecimalExponent
    {
        u64 decimal;
        i32 exponent;
    };

    struct LookupTables
    {
        // NOTE: Range between -32 and 44
        static constexpr u64 Elements = 44 - (-32) + 1;
        static constexpr auto ReversePowerOf10 = []
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

        static constexpr auto H37 = []
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
        static constexpr auto C1 = (((u64)('0' + '0' * 256) << (36)) + (((u64)1 << (36 - 1)) - 7));
#else
        static constexpr auto C1 = (((u64)('0' + '0' * 256) << (36 - 1)) + (((u64)1 << (36 - 2)) - 7));
#endif
        static constexpr auto Div10000 = 1844674407370956;
        static constexpr auto M = (1ULL << 32) - 10000;
        static constexpr auto E7 = 10000000;
        static constexpr auto E6 = 1000000;
        static constexpr auto M32x4 = {0x147b000, -100 + 0x10000, 0xce0, -10 + 0x100};
    };

    struct Decimal8ToAsciiResult
    {
        u64 ascii;
        u64 significantDigitsSub1;
    };

    constexpr Decimal8ToAsciiResult Decimal8ToAsciiAvx512(const u64 m, const u32 upDown, const u32 lz) {}

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
    }

    struct Ascii8
    {
        Containers::Array<char, 24> digits;
        u32 significantDigits;
        i32 exponent;
    };

    SSSENGINE_GLOBAL
    constexpr Ascii8 FloatToAscii(float v)
    {
        //         const struct FloatTable *t = &float_table;
        //         const struct ConstValueFloat *c = &t->constants_float;
        // #if SSSENGINE_ARM64 && (defined(SSSENGINE_INLINE_ASSEMBLY))
        //         // For arm64 processor , fewer instructions
        //         asm("" : "+r"(c)); // read constant values from memory to register
        // #endif
        u32 bits = AsBits(v);

        u32 mantissa = Mantissa<f32>(bits);
        u64 exponent = Exponent<f32>(bits);

        u64 c = mantissa | (1 << FloatTraits<f32>::MantissaWidth);
        SSSENGINE_FUNCTION_LOCAL constexpr i64 ExponentBias = 127 + 23;
        i64 q = (i64)exponent - ExponentBias;

        if(exponent == 0) SSSENGINE_UNLIKELY
        {
            if(mantissa == 0)
            {
                Ascii8 result{};
                result.digits[0] = '0';
                return result;
            }

            q = 1 - ExponentBias;
            c = mantissa;
        }

        if(exponent == 255) SSSENGINE_UNLIKELY
        {
            return {};
            // return (char *)memcpy(buf, mantissa ? "nan" : "inf", 4) + 3;
        }

#ifdef SSSENGINE_AARCH64 // for arm64 processor , fewer instructions
        i64 k = ((i64)biasedExponent * (u128)(1233ull << 52)) >> 64; // signed multiplication
#else
        i64 k = (q * 1233) >> 12;
#endif

        bool irregular = mantissa == 0;
        SSSENGINE_FUNCTION_LOCAL constexpr int Bit = 36;

        u8 h37 = LookupTables::H37[exponent];
        if(irregular) SSSENGINE_UNLIKELY
        {
            k = (i64)(q * 1233 - 512) >> 12;
            h37 = (Bit + 1) + q + ((k * -1701 + (-1701)) >> 9);
        }

        u64 pow10 = LookupTables::ReversePowerOf10[k + 45];
        u64 cb = c << h37;
        u64 hi64 = u128(cb) * pow10 >> 64;
        u64 halfUlp = (pow10 >> (65 - h37)) + ((mantissa + 1) & 1);
        u64 dotOne36Bit = hi64 & ((u64(1) << Bit) - 1);
        u32 mUp = (hi64 + halfUlp) >> Bit;
        u32 upDown = mUp > u32((hi64 - (halfUlp >> 0)) >> Bit);

#ifdef SSSENGINE_AARCH64
        u32 one = (dotOne36Bit * 10 + LookupTables::C1 + (dotOne36Bit >> (Bit - 4))) >> Bit;
#else
        u32 one = (dotOne36Bit * 5 + LookupTables::C1 + (dotOne36Bit >> (Bit - 4))) >> (Bit - 1);
#endif
        if(irregular) SSSENGINE_UNLIKELY
        {
            if((q == 31 - 150) | (q == 214 - 150) | (q == 217 - 150))
            {
                ++one;
            }
            upDown = mUp > ((hi64 - (halfUlp >> 1)) >> Bit);
        }

        u32 lz = (u32(mUp) < u32(LookupTables::E7)) + (u32(mUp) < u32(LookupTables::E6));

        auto ascii = Decimal8ToAscii(mUp, upDown, lz);
        SizeType onePos = 8 - lz;
        i32 e10 = static_cast<i32>(k + (onePos));

        Ascii8 result{.significantDigits = static_cast<u32>(ascii.significantDigitsSub1 + 1), .exponent = e10};

        u64 firstSigPos = 1 - e10;

        auto digits = BitCopy<Containers::Array<char, 8>>(ascii.ascii);
        auto oneDigits = BitCopy<Containers::Array<char, 4>>(one);

        RawMemoryCopy(digits.Data(), result.digits.Data(), 8);
        RawMemoryCopy(oneDigits.Data(), &result.digits[onePos], 2);

#ifdef SSSENGINE_AARCH64
        if(exp == 0) SSSENGINE_UNLIKELY
#endif
            if(mUp < 100'000) SSSENGINE_UNLIKELY
            {
                Containers::Array<char, 8> uArr;
                SSSENGINE_FUNCTION_LOCAL constexpr SizeType Index = 2;
                RawMemoryCopy(&result.digits[Index], uArr.Data(), 8);
                u64 u = BitCopy<u64>(uArr);
                u = System::IsLittleEndian() ? u : Byteswap(u);
                u64 lz = Math::CountRightZeros(u & 0x0f0f0f0f0f0f0f0f) / 8;
                lz += 2;
                e10 -= lz;
                result.digits[0] = result.digits[lz];
                result.exponent = e10;
                result.significantDigits = 8 - lz - 1;
                RawMemoryMove(&result.digits[lz + 1], &result.digits[1], 8);
            }

        return result;
    }

} // namespace SSSEngine::Text
