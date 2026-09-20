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

#include "Byteswap.h"
#include "Concepts.h"
#include "Debug.h"
#include "Encoding.h"
#include "Endian.h"
#include "HelperMacros.h"
#include "Integer.h"
#include "Iterator.h"
#include "Math.h"
#include "Simd.h"
#include "StringView.h"

namespace SSSEngine::Text
{
    template<CharTypeConcept Char>
    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr bool IsDigit(Char digit) noexcept
    {
        return digit >= Char('0') && digit <= Char('9');
    }

    template<CharTypeConcept Char>
    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr bool IsSign(Char digit) noexcept
    {
        return digit == Char('+') || digit == Char('-');
    }

    template<CharTypeConcept Char>
    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr bool SignValue(Char digit) noexcept
    {
        SSSENGINE_ASSERT(IsSign(digit));

        return (digit - 0x2C) * -1;
    }

    template<CharTypeConcept Char>
    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr u64 CharToInt(Char digit) noexcept
    {
        SSSENGINE_ASSERT(IsDigit(digit));

        return digit - Char('0');
    }

    template<Ranges::InputIteratorConcept It>
        requires CharTypeConcept<Ranges::IteratorValueType<It>>
    struct StringToUnsignedResult
    {
        u64 value;
        It it;
    };

    template<Ranges::InputIteratorConcept It>
        requires CharTypeConcept<Ranges::IteratorValueType<It>>
    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr StringToUnsignedResult<It> StringToUnsignedInt(It it, Ranges::SentinelForConcept<It> auto end) noexcept
    {
        u64 value = 0;
        while(it != end && IsDigit(*it))
        {
            value = value * 10 + CharToInt(*it);
            ++it;
        }

        return {value, it};
    }

    template<Ranges::InputIteratorConcept It>
        requires CharTypeConcept<Ranges::IteratorValueType<It>>
    struct StringToSignedResult
    {
        u64 value;
        It it;
    };

    template<Ranges::InputIteratorConcept It>
        requires CharTypeConcept<Ranges::IteratorValueType<It>>
    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr StringToSignedResult<It> StringToInt(It it, Ranges::SentinelForConcept<It> auto end) noexcept
    {
        auto sign = 1;
        if(IsSign(*it))
        {
            sign = SignValue(*it);
            ++it;
        }

        auto [value, last] = StringToUnsignedResult<It>(it, end);

        SSSENGINE_ASSERT(value <= IntTraits<i64>::Max);

        return {static_cast<i64>(value) * sign, last};
    }

    template<EncodingConcept Encoding>
    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr u64 StringToUnsignedInt(StringView<Encoding> string) noexcept
    {
        using CharType = Encoding::CodeUnitType;

        u64 value = 0;
        for(CharType c: string)
        {
            SSSENGINE_ASSERT(IsDigit(c));

            value = value * 10 + CharToInt(c);
        }

        return value;
    }

    template<EncodingConcept Encoding>
    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr i64 StringToInt(StringView<Encoding> string) noexcept
    {
        auto it = string.Begin();

        auto sign = 1;
        if(IsSign(*it))
        {
            sign = SignValue(*it);
            string.TrimLeft();
        }

        return StringToUnsignedInt(string) * sign;
    }

    struct AsciiInt
    {
        Containers::Array<char, IntTraits<u64>::DecimalDigits> digits;
        u8 numberDigits;
    };

    template<IntegralConcept Int>
    constexpr AsciiInt IntToAscii(Int value)
    {
        if(value == 0)
        {
            return {.digits = {'0'}, .numberDigits = 1};
        }

        static constexpr SizeType MaxDigits = IntTraits<u64>::DecimalDigits;

        if constexpr(IsSigned<Int>)
        {
            // TODO: Min of i64 since that cannot be represented by u64
        }

        u64 unsignedValue = [value]
        {
            if constexpr(IsSigned<Int>)
            {
                return static_cast<u64>(Math::Absolute(value));
            }
            else
            {
                return value;
            }
        }();

        AsciiInt result{};
        auto &digits = result.digits;

        SizeType i = MaxDigits;
        for(; unsignedValue > 0; --i)
        {
            digits[i] = char('0' + (unsignedValue % 10));
            unsignedValue /= 10;
        }

        auto written = MaxDigits - i;
        RawMemoryMove(digits.Data() + i + 1, digits.Data(), written);

        result.numberDigits = written;
        return result;
    }

} // namespace SSSEngine::Text
