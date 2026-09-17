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

#include "Concepts.h"
#include "Encoding.h"
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

    template<EncodingConcept Encoding>
    SSSENGINE_CONST SSSENGINE_FORCE_INLINE
    constexpr u64 StringToUnsignedInt(StringView<Encoding> string) noexcept
    {
        using CharType = Encoding::CodeUnitType;

        u64 value = 0;
        for(CharType c: string)
        {
            SSSENGINE_ASSERT(IsDigit(c));
            value = value * 10 + (c - CharType('0'));
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
            sign = (*it - 0x2C) * -1;
            string.TrimLeft();
        }

        return StringToUnsignedInt(string) * sign;
    }

    template<EncodingConcept Encoding, IntegralConcept T, Ranges::OutputIteratorConcept<StringView<Encoding>> Out>
        requires(!IsSameType<T, char>)
    constexpr Out IntToString(T value, Out out)
    {
        using CharType = Encoding::CodeUnitType;

        if(value == 0)
        {
            *out++ = SSSENGINE_ENCODING_SELECTOR(CharType, "0");

            return out;
        }

        if constexpr(IsSigned<T>)
        {
            if(value == IntTraits<T>::Min)
            {
                StringView<Encoding> min = []()
                {
                    if constexpr(IsSameType<T, i8>)
                    {
                        return StringView<Encoding>(SSSENGINE_ENCODING_SELECTOR(CharType, "-128"));
                    }
                    else if constexpr(IsSameType<T, i16>)
                    {
                        return StringView<Encoding>(SSSENGINE_ENCODING_SELECTOR(CharType, "-32768"));
                    }
                    else if constexpr(IsSameType<T, i32>)
                    {
                        return StringView<Encoding>(SSSENGINE_ENCODING_SELECTOR(CharType, "-2147483648"));
                    }
                    else if constexpr(IsSameType<T, i64>)
                    {
                        return StringView<Encoding>(SSSENGINE_ENCODING_SELECTOR(CharType, "-9223372036854775808"));
                    }
                }();
                *out++ = min;
                return out;
            }
        }

        static constexpr SizeType MaxDigits = IntTraits<T>::DecimalDigits + 1;
        CharType tmp[MaxDigits + 1]; // NOTE: Extra 1 for the sign (-)

        using Unsigned = UnsignedType<T>;
        bool isNegative = value < 0;
        auto unsignedValue = [value]
        {
            if constexpr(IsSigned<T>)
            {
                return static_cast<Unsigned>(Math::Absolute(value));
            }
            else
            {
                return value;
            }
        }();

        SizeType i = MaxDigits;
        for(; unsignedValue > 0; --i)
        {
            tmp[i] = CharType('0') + (unsignedValue % 10);
            unsignedValue /= 10;
        }

        if(isNegative)
        {
            tmp[i--] = CharType('-');
        }

        auto written = MaxDigits - i;
        StringView<Encoding> view(tmp + i + 1, written);

        *out++ = view;
        return out;
    }

} // namespace SSSEngine::Text
