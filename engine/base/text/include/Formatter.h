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
 * @brief Helper for formatting strings
 */

#pragma once

#include "Address.h"
#include "Array.h"
#include "AsciiEncoding.h"
#include "Attributes.h"
#include "Concepts.h"
#include "ConversionTraits.h"
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "Encoding.h"
#include "EnumHelpers.h"
#include "Float.h"
#include "FloatParser.h"
#include "HelperMacros.h"
#include "Iterator.h"
#include "Math.h"
#include "MemoryUtility.h"
#include "QualifierTraits.h"
#include "ReverseView.h"
#include "Sink.h"
#include "String.h"
#include "StringView.h"
#include "Traits.h"
#include "Types.h"
#include "Utf8Encoding.h"
#include "IntParser.h"

namespace SSSEngine::Text
{

    /**
     * @class FormatString
     * @brief A string ready for format functions
     *
     */
    template<EncodingConcept Encoding, typename... Args>
    struct FormatString
    {
        template<typename T>
            requires IsConvertible<const T &, StringView<Encoding>>
        consteval FormatString(const T &s) : string{s} // NOLINT(*-explicit-constructor)
        {
            using CharType = Encoding::CodeUnitType;

            SSSENGINE_FUNCTION_LOCAL constexpr SizeType SizeArgs = sizeof...(Args);

            SSSENGINE_FUNCTION_LOCAL constexpr CharType LeftBrace('{');
            SSSENGINE_FUNCTION_LOCAL constexpr CharType RightBrace('}');

            auto it = string.Begin();
            auto end = string.End();
            auto index = 0;

            while(it != end)
            {
                if(*it == LeftBrace)
                {
                    ++it;
                    if(*it == LeftBrace)
                    {
                        ++it;
                        continue;
                    }
                    auto [nextIndex, last] = StringToUnsignedInt(it, end);
                    if(it != last)
                    {
                        index = nextIndex;

                        it = last;
                    }

                    if(index >= SizeArgs)
                    {
                        throw "Invalid Index";
                    }

                    ++index;

                    if(*it != CharType(':') && *it != RightBrace)
                    {
                        throw "Invalid character";
                    }

                    while(*it != RightBrace)
                    {
                        if(*it == LeftBrace)
                        {
                            ++it;
                            auto [nestedIndex, last] = StringToUnsignedInt(it, end);
                            if(it != last)
                            {
                                it = last;
                                if(*it != RightBrace)
                                {
                                    throw "Nested specifier can only have a number";
                                }
                            }
                            else if(*it != RightBrace)
                            {
                                throw "Invalid arg for nested specifier";
                            }

                            index = nestedIndex;
                            if(index >= SizeArgs)
                            {
                                throw "Invalid index for nested specifier";
                            }
                            ++index;
                        }

                        ++it;

                        if(it == end)
                        {
                            throw "End of string reached without a closing brace";
                        }
                    }
                }
                else if(*it == RightBrace)
                {
                    if(*(it + 1) != RightBrace)
                    {
                        throw "No opening left brace for this right brace";
                    }

                    ++it;
                }
                ++it;
            }
        }

        consteval operator StringView<Encoding>() // NOLINT(*-explicit-constructor)
        {
            return string;
        }

        StringView<Encoding> string;
    };

    template<EncodingConcept Encoding>
    class FormatArgs;

    template<EncodingConcept Encoding, typename OutIterator>
    struct FormatContext
    {
        using CharType = Encoding::CodeUnitType;
        OutIterator out;
    };

    template<EncodingConcept Encoding, typename ParseIterator>
    struct ParseContext
    {
        using CharType = Encoding::CodeUnitType;
        ParseIterator out;
        ParseIterator end;
        FormatArgs<Encoding> args;
    };

    // =================================================================================================================
    // Specifiers
    // =================================================================================================================

    enum class Alignment : u8
    {
        None,
        Left,
        Right,
        Center,
    };

    template<EncodingConcept Encoding>
    struct FillAlignmentSpecifier
    {
        using CharType = Encoding::CodeUnitType;

        template<typename ParseCtx>
            requires SameAsConcept<typename ParseCtx::CharType, CharType>
        SSSENGINE_CONST
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            using enum Alignment;

            auto it = ctx.out;

            SSSENGINE_FUNCTION_LOCAL constexpr CharType LeftAlign('<');
            SSSENGINE_FUNCTION_LOCAL constexpr CharType RightAlign('>');
            SSSENGINE_FUNCTION_LOCAL constexpr CharType CenterAlign('^');

            if(*it == LeftAlign)
            {
                alignment = Left;
                return it + 1;
            }

            if(*(it + 1) == LeftAlign)
            {
                alignment = Left;
                fill = *it;
                return it + 2;
            }

            if(*it == RightAlign)
            {
                alignment = Right;
                return it + 1;
            }

            if(*(it + 1) == RightAlign)
            {
                alignment = Right;
                fill = *it;
                return it + 2;
            }

            if(*it == CenterAlign)
            {
                alignment = Center;
                return it + 1;
            }

            if(*(it + 1) == CenterAlign)
            {
                alignment = Center;
                fill = *it;
                return it + 2;
            }

            return it;
        }

        Alignment alignment = Alignment::None;
        // FIXME: This should be a grapheme cluster!
        CharType fill = CharType(' ');
    };

    struct AlternateFormSpecifier
    {
        template<typename ParseCtx>
        SSSENGINE_CONST
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            using CharType = ParseCtx::CharType;

            auto it = ctx.out;

            if(*it != CharType('#'))
            {
                return it;
            }

            alternateForm = true;
            return it + 1;
        }

        SSSENGINE_CONST SSSENGINE_FORCE_INLINE
        constexpr explicit operator bool() const noexcept
        {
            return alternateForm;
        }

        bool alternateForm = false;
    };

    struct ZeroPadSpecifier
    {
        template<typename ParseCtx>
        SSSENGINE_CONST
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            using CharType = ParseCtx::CharType;

            auto it = ctx.out;

            if(*it != CharType('0'))
            {
                return it;
            }

            zeroPad = true;
            return it + 1;
        }

        bool zeroPad = false;
    };

    struct FormSpecifier
    {
        enum class Form : u8
        {
            Regular,
            Binary,
            Hex,
        };

        template<typename ParseCtx>
        SSSENGINE_CONST
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            using CharType = ParseCtx::CharType;

            auto it = ctx.out;

            if(*it == CharType('x') || *it == CharType('X'))
            {
                form = Form::Hex;
                return it + 1;
            }

            if(*it == CharType('b') || *it == CharType('B'))
            {
                form = Form::Binary;
                return it + 1;
            }

            return it;
        }

        Form form = Form::Regular;
    };

    struct WidthSpecifier
    {
        template<typename ParseCtx>
        SSSENGINE_CONST
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            using CharType = ParseCtx::CharType;

            auto it = ctx.out;

            if(*it == CharType('{'))
            {
                ++it;
                auto [n, next] = StringToUnsignedInt(it, ctx.end);
                if(it != next)
                {
                    auto arg = ctx.args.Get(n);
                    arg.Visit(
                        [this](auto &v)
                        {
                            using Type = RemoveReferenceType<decltype(v)>;
                            if constexpr(IntegralConcept<Type>)
                            {
                                width = v;
                            }
                            else
                            {
                                SSSENGINE_UNREACHABLE;
                            }
                        });
                }
                SSSENGINE_ASSERT(*next == CharType('}'));
                return next + 1;
            }
            auto [n, next] = StringToUnsignedInt(it, ctx.end);
            if(it != next)
            {
                it = next;
                width = n;
            }

            return it;
        }

        SizeType width = 0;
    };

    struct PrecisionSpecifier
    {
        template<typename ParseCtx>
        SSSENGINE_CONST
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            using CharType = ParseCtx::CharType;

            auto it = ctx.out;

            if(*it != CharType('.'))
            {
                return it;
            }

            ++it;

            if(*it == CharType('{'))
            {
                ++it;
                auto [n, next] = StringToUnsignedInt(it);
                if(it != next)
                {
                    auto arg = ctx.args.Get(n);
                    arg.Visit(
                        [this](auto &v)
                        {
                            using Type = RemoveReferenceType<decltype(v)>;
                            if constexpr(IntegralConcept<Type>)
                            {
                                precision = v;
                            }
                            else
                            {
                                SSSENGINE_UNREACHABLE;
                            }
                        });
                }
                SSSENGINE_ASSERT(*next == CharType('}'));
                return next + 1;
            }
            auto [n, next] = StringToUnsignedInt(it);
            if(it != next)
            {
                it = next;
                precision = n;
            }

            return it;
        }

        SizeType precision;
    };

    struct SignSpecifier
    {
        enum class Sign : u8
        {
            Negative,
            Always,
            Space
        };

        Sign s = Sign::Negative;

        template<typename ParseCtx>
        SSSENGINE_CONST
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            using CharType = ParseCtx::CharType;
            using enum Sign;

            auto it = ctx.out;

            if(*it == CharType('+'))
            {
                s = Always;
                ++it;
            }
            else if(*it == CharType(' '))
            {
                s = Space;
                ++it;
            }
            else if(*it == CharType('-'))
            {
                s = Negative;
                ++it;
            }

            return it;
        }
    };

    // =================================================================================================================
    // Formatters
    // =================================================================================================================

    template<typename T, EncodingConcept Encoding>
    struct Formatter
    {
        Formatter() = delete;
        Formatter(const Formatter &) = delete;
        Formatter(Formatter &&) = delete;
        Formatter &operator=(const Formatter &) = delete;
        Formatter &operator=(Formatter &&) = delete;
        ~Formatter() = delete;
    };

    template<EncodingConcept Encoding>
    struct Formatter<typename Encoding::CodeUnitType, Encoding>
    {
        using CharType = Encoding::CodeUnitType;

        template<typename ParseCtx>
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            return ctx.out;
        }

        template<typename FmtCtx>
        constexpr auto Format(CharType value, FmtCtx &ctx) const noexcept -> decltype(ctx.out)
        {
            *ctx.out++ = StringView<Encoding>(&value, 1);
            return ctx.out;
        }
    };

    template<EncodingConcept Encoding>
    struct Formatter<StringView<Encoding>, Encoding>
    {
        template<typename ParseCtx>
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            return ctx.out;
        }

        template<typename FmtCtx>
        constexpr auto Format(StringView<Encoding> value, FmtCtx &ctx) const noexcept -> decltype(ctx.out)
        {
            auto it = Move(ctx.out);
            *it++ = value;
            return it;
        }
    };

    template<IntegralConcept Int, EncodingConcept Encoding>
    struct Formatter<Int, Encoding>
    {
        using CharType = Encoding::CodeUnitType;

        template<typename ParseCtx>
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            ctx.out = Move(fillAlign.Parse(ctx));
            ctx.out = Move(sign.Parse(ctx));
            ctx.out = Move(alternateForm.Parse(ctx));
            ctx.out = Move(zeroPad.Parse(ctx));
            ctx.out = Move(width.Parse(ctx));
            ctx.out = Move(form.Parse(ctx));

            SSSENGINE_ASSERT(*ctx.out == CharType('}'));

            return ctx.out;
        }

        template<typename FmtCtx>
        constexpr auto Format(Int value, FmtCtx &ctx) const noexcept
        {
            using enum FormSpecifier::Form;
            AsciiInt ascii = [this, value]
            {
                switch(form.form)
                {
                    case Regular:
                        return IntToAscii(value);
                    case Binary:
                        return IntToBinaryAscii(value);
                    case Hex:
                        return IntToHexAscii(value);
                    default:
                        SSSENGINE_UNREACHABLE;
                }
            }();

            // TODO: What size should it be?
            char tmp[32]{};
            char *it = tmp;

            const auto fill = [this](char *it, SizeType amount) { MemorySet(it, fillAlign.fill, amount); };

            using enum SignSpecifier::Sign;

            auto valueSign = Math::SignOf(value);

            if(valueSign == -1)
            {
                *it++ = '-';
            }
            else if(valueSign == 1)
            {
                switch(sign.s)
                {
                    case Always:
                        *it++ = '+';
                        break;
                    case Space:
                        *it++ = ' ';
                        break;
                    case Negative:
                        break;
                    default:
                        SSSENGINE_UNREACHABLE;
                }
            }
            if(alternateForm)
            {
                switch(form.form)
                {
                    case Regular:
                        break;
                    case Binary:
                        *it++ = '0';
                        *it++ = 'b';
                        break;
                    case Hex:
                        *it++ = '0';
                        *it++ = 'x';
                        break;
                    default:
                        SSSENGINE_UNREACHABLE;
                }
            }
            auto endPrefix = it;
            RawMemoryCopy(ascii.digits.Data(), it, ascii.numberDigits);
            it += ascii.numberDigits;

            SizeType total = it - tmp;
            i32 fillAmount = i32(width.width - total);
            if(fillAmount > 0)
            {
                switch(fillAlign.alignment)
                {
                    case Alignment::None:
                        if(zeroPad.zeroPad)
                        {
                            RawMemoryMove(endPrefix, endPrefix + fillAmount, total);
                            MemorySet(endPrefix, '0', fillAmount);
                            break;
                        }
                        else
                            SSSENGINE_FALLTHROUGH;
                    case Alignment::Right:
                    {
                        RawMemoryMove(tmp, tmp + fillAmount, total);
                        fill(tmp, fillAmount);
                        break;
                    }
                    case Alignment::Left:
                    {
                        fill(it, total);
                        break;
                    }
                    case Alignment::Center:
                    {
                        auto half = fillAmount / 2;
                        auto otherHalf = fillAmount - half;

                        RawMemoryMove(tmp, tmp + half, total);
                        fill(tmp, half);
                        fill(it + half, otherHalf);
                    }
                    break;
                    default:
                        SSSENGINE_UNREACHABLE;
                }

                total = width.width;
            }

            *ctx.out++ = StringView<Encoding>{tmp, total};

            return ctx.out;
        }

        FillAlignmentSpecifier<Encoding> fillAlign;
        SignSpecifier sign;
        AlternateFormSpecifier alternateForm;
        ZeroPadSpecifier zeroPad;
        WidthSpecifier width;
        FormSpecifier form;
    };

    template<FloatingPointConcept Float, EncodingConcept Encoding>
    struct Formatter<Float, Encoding>
    {
        using CharType = Encoding::CodeUnitType;

        template<typename ParseCtx>
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            return ctx.out;
        }

        template<typename FmtCtx>
        constexpr auto Format(Float value, FmtCtx &ctx) const noexcept
        {
            SSSENGINE_FUNCTION_LOCAL constexpr StringView<Encoding> Zero = SSSENGINE_ENCODING_SELECTOR(CharType, "0");
            SSSENGINE_FUNCTION_LOCAL constexpr StringView<Encoding> NaN = SSSENGINE_ENCODING_SELECTOR(CharType, "NaN");
            SSSENGINE_FUNCTION_LOCAL constexpr StringView<Encoding> Inf = SSSENGINE_ENCODING_SELECTOR(CharType, "Infinity");
            SSSENGINE_FUNCTION_LOCAL constexpr StringView<Encoding> SignedInf[] = {
                SSSENGINE_ENCODING_SELECTOR(CharType, "+Infinity"), SSSENGINE_ENCODING_SELECTOR(CharType, "-Infinity")};

            if(value == 0) SSSENGINE_UNLIKELY
            {
                *ctx.out++ = Zero;
                return ctx.out;
            }
            if(IsNaN(value)) SSSENGINE_UNLIKELY
            {
                *ctx.out++ = NaN;
                return ctx.out;
            }
            if(IsInfinity(value)) SSSENGINE_UNLIKELY
            {
                auto showSign = SignBit(value);
                if(showSign)
                {
                    *ctx.out++ = SignedInf[showSign];
                }
                else
                {
                    *ctx.out++ = Inf;
                }
                return ctx.out;
            }

            auto decimal = FloatToAscii(value);
            ctx.out = Move(FormatShort(value, decimal, ctx));
            return ctx.out;
        }

      private:
        static constexpr char Signs[] = {'+', '-'};

        template<typename FmtCtx>
        constexpr auto FormatShort(f64 value, FloatToAsciiResult &decimal, FmtCtx &ctx) const noexcept
        {
            auto sign = SignBit(value);
            u32 showSign = sign;
            u32 hideSign = !sign;

            i32 positiveExponent = decimal.exponent >= 0;
            u32 first = (positiveExponent ? 0 : 1 - decimal.exponent) + 1;
            RawMemoryMove(decimal.digits.Data(), &decimal.digits[first], decimal.significantDigits);

            auto dot = first + decimal.exponent + positiveExponent;
            auto move = positiveExponent ? dot + 1 : dot;
            RawMemoryMove(&decimal.digits[dot], &decimal.digits[move], 8);

            for(SizeType i = showSign; i < first; ++i)
            {
                decimal.digits[i] = '0';
            }

            decimal.digits[0] = Signs[sign];
            decimal.digits[dot] = '.';

            auto countNegative = first - hideSign + decimal.significantDigits;

            u32 absoluteExponent = Math::Absolute(decimal.exponent) + 1;
            u32 digits = Math::Max(decimal.significantDigits, absoluteExponent);
            bool hasDot = decimal.significantDigits > absoluteExponent;

            auto count = positiveExponent ? digits + hasDot + showSign : countNegative;

            StringView<Encoding> view(&decimal.digits[hideSign], count);
            *ctx.out++ = view;

            return ctx.out;
        }
    };

    template<EncodingConcept Encoding>
    struct Formatter<bool, Encoding>
    {
        using CharType = Encoding::CodeUnitType;

        template<typename ParseCtx>
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            return ctx.out;
        }

        template<typename FmtCtx>
        constexpr auto Format(bool value, FmtCtx &ctx) const noexcept
        {
            SSSENGINE_FUNCTION_LOCAL constexpr StringView<Encoding> True = SSSENGINE_ENCODING_SELECTOR(CharType, "True");
            SSSENGINE_FUNCTION_LOCAL constexpr StringView<Encoding> False = SSSENGINE_ENCODING_SELECTOR(CharType, "False");

            *ctx.out++ = value ? True : False;
            return ctx.out;
        }
    };

    template<EncodingConcept Encoding>
    struct Formatter<void *, Encoding>
    {
        using CharType = Encoding::CodeUnitType;

        template<typename ParseCtx>
        constexpr auto Parse(ParseCtx &ctx) noexcept
        {
            return ctx.out;
        }

        template<typename FmtCtx>
        constexpr auto Format(void *value, FmtCtx &ctx) const noexcept
        {
            FormatTo<Encoding>(ctx.out, SSSENGINE_ENCODING_SELECTOR(CharType, "{:#10x}"), reinterpret_cast<u64>(value));
            return ctx.out;
        }
    };

    enum class ArgType : u8
    {
        Bool,
        Char,
        String,
        Int32,
        Int64,
        UInt32,
        UInt64,
        Float32,
        Float64,
        Pointer,
        Custom,

        // TODO: Use reflection to get the number
        Count,
    };

    struct CustomType
    {
        void *data;
        void (*format)();
    };

    template<EncodingConcept Encoding>
    struct FormatArgValue
    {
        using CharType = Encoding::CodeUnitType;

        union
        {
            bool boolean;
            CharType character;
            StringView<Encoding> string;
            i32 i32;
            i64 i64;
            u32 u32;
            u64 u64;
            f32 f32;
            f64 f64;
            void *pointer;
            CustomType custom;
        };
    };

    template<EncodingConcept Encoding, typename T>
    consteval auto NormalizeArgType() noexcept
    {
        using Type = RemoveConstType<T>;
        using CharType = Encoding::CodeUnitType;

        if constexpr(IsSameType<Type, bool>)
        {
            return Identity<bool>{};
        }
        else if constexpr(IsSameType<Type, CharType>)
        {
            return Identity<CharType>{};
        }
        else if constexpr(IsConvertible<Type, StringView<Encoding>>)
        {
            return Identity<StringView<Encoding>>{};
        }
        else if constexpr(IsSameType<DecayType<Type>, CharType *> || IsSameType<DecayType<Type>, const CharType *>)
        {
            // TODO: StringView? Shouldn't the IsConvertible already make this never
            // happen?
            return Identity<const CharType *>{};
        }
        else if constexpr(SignedIntegralConcept<Type>)
        {
            if constexpr(sizeof(Type) <= sizeof(i32))
            {
                return Identity<i32>{};
            }
            else if constexpr(sizeof(Type) <= sizeof(i64))
            {
                return Identity<i64>{};
            }
            else
            {
                SSSENGINE_NOT_IMPLEMENTED;
            }
        }
        else if constexpr(UnsignedIntegralConcept<Type>)
        {
            if constexpr(sizeof(Type) <= sizeof(u32))
            {
                return Identity<u32>{};
            }
            else if constexpr(sizeof(Type) <= sizeof(u64))
            {
                return Identity<u64>{};
            }
            else
            {
                SSSENGINE_NOT_IMPLEMENTED;
            }
        }
        else if constexpr(IsSameType<Type, f32>)
        {
            return Identity<f32>{};
        }
        else if constexpr(IsSameType<Type, f64>)
        {
            return Identity<f64>{};
        }
        else if constexpr(IsPointer<Type>)
        {
            return Identity<void *>{};
        }
        else
        {
            return Identity<CustomType>{};
        }
    }

    template<EncodingConcept Encoding, typename T>
    using NormalizedArgType = decltype(NormalizeArgType<Encoding, T>())::Type;

    template<EncodingConcept Encoding, typename T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr ArgType AsArgType() noexcept
    {
        using enum ArgType;
        using CharType = Encoding::CodeUnitType;
        using Type = NormalizedArgType<Encoding, T>;

        if constexpr(IsSameType<Type, bool>)
        {
            return Bool;
        }
        else if constexpr(IsSameType<Type, CharType>)
        {
            return Char;
        }
        else if constexpr(IsConvertible<Type, StringView<Encoding>>)
        {
            return String;
        }
        else if constexpr(IsSameType<Type, i32>)
        {
            return Int32;
        }
        else if constexpr(IsSameType<Type, i64>)
        {
            return Int64;
        }
        else if constexpr(IsSameType<Type, u32>)
        {
            return UInt32;
        }
        else if constexpr(IsSameType<Type, u64>)
        {
            return UInt64;
        }
        else if constexpr(IsSameType<Type, f32>)
        {
            return Float32;
        }
        else if constexpr(IsSameType<Type, f64>)
        {
            return Float64;
        }
        else if constexpr(IsPointer<Type>)
        {
            return Pointer;
        }
        else if constexpr(IsSameType<Type, CustomType>)
        {
            return Custom;
        }
        else
        {
            SSSENGINE_NOT_IMPLEMENTED;
        }
    }

    template<EncodingConcept Encoding, typename T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArgValue<Encoding> AsArgValue(T &value)
    {
        using Type = NormalizedArgType<Encoding, T>;
        using CharType = Encoding::CodeUnitType;

        if constexpr(IsSameType<Type, bool>)
        {
            return {.boolean = value};
        }
        else if constexpr(IsSameType<Type, CharType>)
        {
            return {.character = value};
        }
        else if constexpr(IsConvertible<Type, StringView<Encoding>>)
        {
            return {.string = value};
        }
        else if constexpr(IsSameType<Type, i32>)
        {
            return {.i32 = i32(value)};
        }
        else if constexpr(IsSameType<Type, i64>)
        {
            return {.i64 = i64(value)};
        }
        else if constexpr(IsSameType<Type, u32>)
        {
            return {.u32 = u32(value)};
        }
        else if constexpr(IsSameType<Type, u64>)
        {
            return {.u64 = u64(value)};
        }
        else if constexpr(IsSameType<Type, f32>)
        {
            return {.f32 = value};
        }
        else if constexpr(IsSameType<Type, f64>)
        {
            return {.f64 = value};
        }
        else if constexpr(IsPointer<Type>)
        {
            return {.pointer = value};
        }
        else if constexpr(IsSameType<Type, CustomType>)
        {
            CustomType c{.data = AddressOf(value), .format = &Formatter<T, Encoding>::Format};
            return {.custom = c};
        }
        else
        {
            SSSENGINE_NOT_IMPLEMENTED;
        }
    }

    template<EncodingConcept Encoding>
    class FormatArg
    {
      public:
        template<typename T>
        SSSENGINE_FORCE_INLINE
        constexpr explicit FormatArg(T value) :
            m_value{AsArgValue<Encoding>(value)}, m_type(AsArgType<Encoding, T>())
        {
        }

        SSSENGINE_FORCE_INLINE
        constexpr FormatArg(FormatArgValue<Encoding> value, ArgType type) :
            m_value{value}, m_type(type)
        {
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr FormatArgValue<Encoding> Value() const noexcept
        {
            return m_value;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ArgType Type() const noexcept
        {
            return m_type;
        }

        template<typename Visitor>
        constexpr decltype(auto) Visit(Visitor &&visitor) noexcept
        {
            switch(m_type)
            {
                case ArgType::Bool:
                    return Forward<Visitor>(visitor)(m_value.boolean);
                case ArgType::Char:
                    return Forward<Visitor>(visitor)(m_value.character);
                case ArgType::String:
                    return Forward<Visitor>(visitor)(m_value.string);
                case ArgType::Int32:
                    return Forward<Visitor>(visitor)(m_value.i32);
                case ArgType::Int64:
                    return Forward<Visitor>(visitor)(m_value.i64);
                case ArgType::UInt32:
                    return Forward<Visitor>(visitor)(m_value.u32);
                case ArgType::UInt64:
                    return Forward<Visitor>(visitor)(m_value.u64);
                case ArgType::Float32:
                    return Forward<Visitor>(visitor)(m_value.f32);
                case ArgType::Float64:
                    return Forward<Visitor>(visitor)(m_value.f64);
                case ArgType::Pointer:
                    return Forward<Visitor>(visitor)(m_value.pointer);
                case ArgType::Custom:
                    return Forward<Visitor>(visitor)(m_value.custom);
                case ArgType::Count:
                    SSSENGINE_FALLTHROUGH;
                default:
                    SSSENGINE_ASSERT("Not a valid type");
                    break;
            }
        }

      private:
        FormatArgValue<Encoding> m_value;
        ArgType m_type;
    };

    template<EncodingConcept Encoding, typename... Args>
    struct FormatArgStorage;

    template<EncodingConcept Encoding>
    class FormatArgs
    {
      public:
        static constexpr SizeType PackedTypeBits = Math::BitWidth(SizeType(ArgType::Count));
        static constexpr SizeType PackedTypeMask = (1 << PackedTypeBits) - 1;

        static constexpr SizeType PackedSizeBits = 4;
        static constexpr SizeType PackedTypesBits = Bits<u64> - 4;

        static constexpr SizeType MaxPackedArgs = PackedTypesBits / PackedTypeBits;

        template<typename... Args>
        constexpr FormatArgs(const FormatArgStorage<Encoding, Args...> &storage) noexcept // NOLINT(*-explicit-constructor)
        {
            constexpr auto Size = sizeof...(Args);
            if constexpr(Size == 0)
            {
                m_packedSize = 0;
                m_packedTypes = 0;
                m_values = nullptr;
            }
            else if constexpr(Size <= MaxPackedArgs)
            {
                m_packedSize = Size;

                u64 types = 0;
                static constexpr Containers::Array<ArgType, sizeof...(Args)> Types{AsArgType<Encoding, Args>()...};
                for(ArgType current: Ranges::Reverse(Types))
                {
                    types = (types << PackedTypeBits) | AsNumber(current);
                }

                m_packedTypes = types;
                m_values = storage.elements;
            }
            else
            {
                m_packedSize = 0;
                m_packedTypes = Size;
                m_args = storage.elements;
            }
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr SizeType Count() const noexcept
        {
            return m_packedSize ? m_packedSize : m_packedTypes;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ArgType GetType(SizeType index) const noexcept
        {
            u64 value = m_packedTypes >> (index * PackedTypeBits);
            return static_cast<ArgType>(value & PackedTypeMask);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr FormatArg<Encoding> Get(SizeType index) const noexcept
        {
            if(index < m_packedSize)
            {
                return {m_values[index], GetType(index)};
            }

            if(m_packedSize == 0 && index < m_packedTypes)
            {
                return m_args[index];
            }

            SSSENGINE_UNREACHABLE;
        }

      private:
        u64 m_packedSize : PackedSizeBits;
        u64 m_packedTypes : PackedTypesBits;

        union
        {
            // NOLINTBEGIN(readability-identifier-naming) These are still private of the
            // class even if they are public for the union

            const FormatArgValue<Encoding> *m_values;
            const FormatArg<Encoding> *m_args;

            // NOLINTEND(readability-identifier-naming)
        };
    };

    using Utf8Args = FormatArgs<Utf8Encoding>;
    using AsciiArgs = FormatArgs<AsciiEncoding>;

    template<EncodingConcept Encoding, typename... Args>
    struct FormatArgStorage
    {
        static constexpr SizeType ArgCount = sizeof...(Args);
        static constexpr bool PackTypes = ArgCount <= FormatArgs<Encoding>::MaxPackedArgs;

        using ElementType = ConditionalType<PackTypes, FormatArgValue<Encoding>, FormatArg<Encoding>>;

        template<typename T>
        constexpr static ElementType MakeElement(T &value) noexcept
        {
            using Type = NormalizedArgType<Encoding, RemoveConstType<T>>;
            SSSENGINE_STATIC_ASSERT(IsDefaultConstructible<Formatter<Type, Encoding>>, "Formatter must be specialized");

            if constexpr(PackTypes)
            {
                return AsArgValue<Encoding>(value);
            }
            else
            {
                return {value};
            }
        }

        ElementType elements[ArgCount];
    };

    template<EncodingConcept Encoding>
    struct FormatArgStorage<Encoding>
    {
    };

    template<EncodingConcept Encoding, typename... Args>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto MakeFormatArgs(Args &...args) noexcept
    {
        using Storage = FormatArgStorage<Encoding, NormalizedArgType<Encoding, Args>...>;
        return Storage{Storage::MakeElement(args)...};
    }

    template<EncodingConcept Encoding, Ranges::OutputIteratorConcept<StringView<Encoding>> OutIterator>
    constexpr void VFormatTo(OutIterator out, StringView<Encoding> fmt, FormatArgs<Encoding> args) noexcept
    {
        using CharType = typename Encoding::CodeUnitType;
        using View = StringView<Encoding>;
        using It = View::Iterator;

        static constexpr auto Left = CharType('{');
        static constexpr auto Right = CharType('}');

        FormatContext<Encoding, OutIterator> fmtCtx{out};
        ParseContext<Encoding, It> parseCtx{fmt.Begin(), fmt.End(), args};

        auto left = fmt.Begin();
        auto it = left;
        auto end = fmt.End();
        auto index = 0;

        while(it != end)
        {
            if(*it == Left)
            {
                ++it;

                if(*it == Left)
                {
                    View view{left, SizeType(it - left)};
                    *fmtCtx.out++ = view;
                    left = it + 1;
                    continue;
                }
                View view{left, SizeType(it - 1 - left)};
                *fmtCtx.out++ = view;

                auto [newIndex, newIt] = StringToUnsignedInt(it, end);
                if(newIt != it)
                {
                    it = newIt;
                    index = newIndex;
                }

                if(*it == CharType(':'))
                {
                    ++it;
                }

                parseCtx.out = Move(it);
                FormatArg<Encoding> type = args.Get(index++);
                type.Visit(
                    [&fmtCtx, &parseCtx](auto &arg)
                    {
                        using Type = RemoveReferenceType<decltype(arg)>;
                        using Formatter = Formatter<Type, Encoding>;

                        if constexpr(IsSameType<Type, CustomType>)
                        {
                            arg.format();
                        }
                        else if constexpr(IsDefaultConstructible<Formatter>)
                        {
                            Formatter fmt;
                            parseCtx.out = Move(fmt.Parse(parseCtx));
                            fmtCtx.out = Move(fmt.Format(arg, fmtCtx));
                        }
                        else
                        {
                            SSSENGINE_STATIC_ASSERT(false, "No way to format");
                        }
                    });

                it = Move(parseCtx.out);
                left = it + 1;
            }
            else if(*it == Right)
            {
                SSSENGINE_ASSERT(*(it + 1) == Right);

                ++it;
                View view{left, SizeType(it - left)};
                *fmtCtx.out++ = view;
                left = it + 1;
            }
            ++it;
        }

        StringView<Encoding> view(left, end - left);
        *fmtCtx.out++ = view;
    }

    template<EncodingConcept Encoding, Ranges::OutputIteratorConcept<StringView<Encoding>> OutIterator, typename... Args>
    constexpr void FormatTo(OutIterator out, FormatString<Encoding, IdentityType<Args>...> fmt, Args &&...args) noexcept
    {
        return VFormatTo(out, fmt.string, FormatArgs<Encoding>(MakeFormatArgs<Encoding>(args...)));
    }

    template<EncodingConcept Encoding>
    constexpr String<Encoding> VFormat(StringView<Encoding> fmt, FormatArgs<Encoding> args) noexcept
    {
        Containers::StringSink<Encoding> sink;
        VFormatTo(sink.Out(), fmt, args);

        return Move(sink).Get();
    }

    // LOW_PRIORITY: Better documentation here:
    /**
     * @brief Formats a string replacing {} by the variable declared in order. Can
     * also use an Id like so {1} representing the index of the arg to use to
     * replace. And can use format options like so {:fmt}. Can do both as well
     * {1:fmt}
     *
     * @param format A string representing the format
     * @param args The variables that will replace the {} on the format string
     * @return A formatted string
     */
    template<EncodingConcept Encoding, typename... Args>
    constexpr String<Encoding> Format(FormatString<Encoding, IdentityType<Args>...> fmt, Args &&...args) noexcept
    {
        if constexpr(sizeof...(args) == 0)
        {
            return String<Encoding>{fmt.string};
        }
        else
        {
            auto fmtArgs = FormatArgs<Encoding>(MakeFormatArgs<Encoding>(args...));
            return VFormat(fmt.string, fmtArgs);
        }
    }

    /**
     * @brief Converts a value to a string using it's default formatter
     *
     * @tparam Encoding The encoding to use
     * @tparam T The type of the value to format
     * @param value The value to convert
     * @return A string converted from value
     */
    template<EncodingConcept Encoding, typename T>
    constexpr String<Encoding> ToString(T &value) noexcept
    {
        using Type = NormalizedArgType<Encoding, T>;
        using Formatter = Formatter<Type, Encoding>;

        Containers::StringSink<Encoding> sink;
        FormatContext<Encoding, decltype(sink.Out())> fmtCtx{sink.Out()};
        Formatter fmt;
        fmt.Format(value, fmtCtx);

        return Move(sink).Get();
    }

} // namespace SSSEngine::Text
