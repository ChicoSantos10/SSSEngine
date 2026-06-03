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

#include "Algorithm.h"
#include "AsciiEncoding.h"
#include "Attributes.h"
#include "Bits.h"
#include "Concepts.h"
#include "ConversionTraits.h"
#include "Debug.h"
#include "EnumHelpers.h"
#include "Iterator.h"
#include "Limits.h"
#include "Math.h"
#include "MemoryUtility.h"
#include "QualifierTraits.h"
#include "SignTraits.h"
#include "String.h"
#include "StringSink.h"
#include "StringView.h"
#include "Encoding.h"
#include "Traits.h"
#include "Types.h"
#include "Address.h"
#include "Utf8Encoding.h"
#include "HelperMacros.h"

#define SSSENGINE_ENCODING_SELECTOR(charType, message)                                                                 \
    []() -> auto                                                                                                       \
    {                                                                                                                  \
        if constexpr(IsSameType<charType, char>)                                                                       \
        {                                                                                                              \
            return SSSENGINE_ASCII(message);                                                                           \
        }                                                                                                              \
        else if constexpr(IsSameType<charType, char8>)                                                                 \
        {                                                                                                              \
            return SSSENGINE_UTF8(message);                                                                            \
        }                                                                                                              \
        else if constexpr(IsSameType<charType, char16>)                                                                \
        {                                                                                                              \
            return SSSENGINE_UTF16(message);                                                                           \
        }                                                                                                              \
        else if constexpr(IsSameType<charType, char32>)                                                                \
        {                                                                                                              \
            return SSSENGINE_UTF32(message);                                                                           \
        }                                                                                                              \
        SSSENGINE_UNREACHABLE;                                                                                         \
    }()

namespace SSSEngine::Text
{
    template<EncodingConcept Encoding, SignedIntegralConcept T>
    constexpr void ParseInt(T value, typename Encoding::CodeUnitType *buffer)
    {
        using CharType = Encoding::CodeUnitType;

        if(value == 0)
        {
            buffer[0] = SSSENGINE_ENCODING_SELECTOR(CharType, '0');

            return;
        }

        if(value == Math::Limits::Min<T>)
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
            MemoryCopy(min.Data(), buffer, min.Count());
            return;
        }

        CharType tmp[Math::Limits::DecimalDigits<T> + 1]; // NOTE: Extra 1 for the sign (-)

        using Unsigned = UnsignedType<T>;
        bool isNegative = value < 0;
        Unsigned unsignedValue = isNegative ? static_cast<Unsigned>(-value) : static_cast<Unsigned>(value);

        SizeType i = 0;
        for(; unsignedValue > 0; ++i)
        {
            tmp[i] = CharType('0') + (unsignedValue % 10);
            unsignedValue /= 10;
        }

        SizeType out = 0;
        if(isNegative)
        {
            buffer[out++] = CharType('-');
        }

        ReverseMemoryCopy(tmp, &buffer[out], i);
    }

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
        constexpr FormatString(const T &string) : string{string} // NOLINT(*-explicit-constructor)
        {
            if consteval
            {
                // TODO: Validate string
            }
        }

        constexpr operator StringView<Encoding>() // NOLINT(*-explicit-constructor)
        {
            return string;
        }

        StringView<Encoding> string;
    };

    template<EncodingConcept Encoding, Ranges::OutputIteratorConcept<typename Encoding::CodeUnitType> Out>
    struct FormatContext
    {
        Out output;
    };

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

    template<IntegralConcept Int, EncodingConcept Encoding>
    struct Formatter<Int, Encoding>
    {
        using CharType = Encoding::CodeUnitType;

        constexpr auto Parse() const noexcept
        {
            // TODO: Parse
        }

        template<typename FmtCtx>
        constexpr auto Format(Int value, FmtCtx &ctx) const noexcept
        {
            ParseInt<Encoding>(value, ctx.output);
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
            return Identity<const void *>{};
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
    constexpr ArgType AsArgType()
    {
        using enum ArgType;
        using CharType = Encoding::CodeUnitType;

        if constexpr(IsSameType<T, bool>)
        {
            return Bool;
        }
        else if constexpr(IsSameType<T, CharType>)
        {
            return Char;
        }
        else if constexpr(IsSameType<T, i32>)
        {
            return Int32;
        }
        else if constexpr(IsSameType<T, i64>)
        {
            return Int64;
        }
        else if constexpr(IsSameType<T, u32>)
        {
            return UInt32;
        }
        else if constexpr(IsSameType<T, u64>)
        {
            return UInt64;
        }
        else if constexpr(IsSameType<T, f32>)
        {
            return Float32;
        }
        else if constexpr(IsSameType<T, f64>)
        {
            return Float64;
        }
        else if constexpr(IsPointer<T>)
        {
            return Pointer;
        }
        else if constexpr(IsSameType<T, CustomType>)
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
        using CharType = Encoding::CodeUnitType;

        if constexpr(IsSameType<T, bool>)
        {
            return {.boolean = value};
        }
        else if constexpr(IsSameType<T, CharType>)
        {
            return {.character = value};
        }
        else if constexpr(IsSameType<T, i32>)
        {
            return {.i32 = i32(value)};
        }
        else if constexpr(IsSameType<T, i64>)
        {
            return {.i64 = i64(value)};
        }
        else if constexpr(IsSameType<T, u32>)
        {
            return {.u32 = u32(value)};
        }
        else if constexpr(IsSameType<T, u64>)
        {
            return {.u64 = u64(value)};
        }
        else if constexpr(IsSameType<T, f32>)
        {
            return {.f32 = value};
        }
        else if constexpr(IsSameType<T, f64>)
        {
            return {.f64 = value};
        }
        else if constexpr(IsPointer<T>)
        {
            return {.pointer = value};
        }
        else if constexpr(IsSameType<T, CustomType>)
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

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        FormatArgValue<Encoding> Value() const noexcept
        {
            return m_value;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        ArgType Type() const noexcept
        {
            return m_type;
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
        static constexpr SizeType PackedTypesBits = Math::Limits::Bits<u64> - 4;

        static constexpr SizeType MaxPackedArgs = PackedTypesBits / PackedTypeBits;

        template<typename... Args>
        FormatArgs(const FormatArgStorage<Encoding, Args...> &storage) noexcept // NOLINT(*-explicit-constructor)
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

                m_packedTypes = 0;
                // TODO: Replace with array and reverse for each
                constexpr ArgType Types[]{AsArgType<Encoding, Args>()...};
                for(auto current = Types + Size - 1; current != Types - 1; --current)
                {
                    m_packedTypes = m_packedTypes << PackedTypeBits | AsNumber(*current);
                }

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
        SizeType Count() const noexcept
        {
            return m_packedSize ? m_packedSize : m_packedTypes;
        }

        SSSENGINE_PURE
        ArgType GetType(SizeType index) const noexcept
        {
            u64 value = m_packedTypes >> (index * PackedTypeBits);
            return static_cast<ArgType>(value & PackedTypeMask);
        }

      private:
        u64 m_packedSize : PackedSizeBits;
        u64 m_packedTypes : PackedTypesBits;

        union
        {
            // NOLINTBEGIN(readability-identifier-naming) These are still private of the class even if they are
            // public for the union

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
        static ElementType MakeElement(T &value) noexcept
        {
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
    constexpr FormatArgs<Encoding> MakeFormatArgs(Args &...args)
    {
        using Storage = FormatArgStorage<Encoding, NormalizedArgType<Encoding, Args>...>;
        return Storage{Storage::MakeElement(args)...};
    }

    template<EncodingConcept Encoding, SinkConcept Sink>
    void FormatTo(Sink &out, StringView<Encoding> fmt, FormatArgs<Encoding> args)
    {
        using namespace Iterators;

        using CharType = typename Encoding::CodeUnitType;
        using It = StringView<Encoding>::Iterator;

        static constexpr auto Left = CharType('{');
        static constexpr auto Right = CharType('}');

        const auto end = fmt.End();
        const auto findArgBegin = [end](It begin) -> It
        {
            auto current = begin;
            while(true)
            {
                auto it = Find(current, end, Left);
                if(it == end)
                {
                    return end;
                }

                if(*(it + 1) == Left)
                {
                    current = it + 2;
                    continue;
                }

                return it;
            }
        };

        const auto findArgEnd = [end](It begin) -> It
        {
            auto current = begin;
            while(true)
            {
                auto it = Find(current, end, Right);

                // NOTE: A properly validated string always has an end to a started arg
                SSSENGINE_ASSERT(it != end);

                if(*(it + 1) == Right)
                {
                    current = it + 2;
                    continue;
                }

                return it;
            }
        };

        It argBegin;
        SizeType argIndex = 0;
        while(argBegin = findArgBegin(fmt.Begin()), argBegin != end)
        {
            const auto argEnd = findArgEnd(argBegin);

            // TODO:
            // - Copy from format to sink
            // - Parse Context: {id:opts}
            //      - Get optional Arg index or increment
            //      - Get other possible values
            // - Format into sink
        }
    }

    template<EncodingConcept Encoding>
    String<Encoding> FormatEngine(StringView<Encoding> fmt, FormatArgs<Encoding> args)
    {
        DirectSink<String<Encoding>> sink;
        FormatTo(sink, fmt, args);

        return Move(sink).Get();
    }

    // LOW_PRIORITY: Better documentation here:
    /**
     * @brief Formats a string replacing {} by the variable declared in order. Can also use an Id like so {1}
     * representing the index of the arg to use to replace. And can use format options like so {:fmt}. Can do both
     * as well {1:fmt}
     *
     * @param format A string representing the format
     * @param args The variables that will replace the {} on the format string
     * @return A formatted string
     */
    template<EncodingConcept Encoding, typename... Args>
    String<Encoding> Format(FormatString<Encoding, IdentityType<Args>...> fmt, Args &&...args)
    {
        // Find replacement fields: {} which can have an Id and/or a format spec {id:spec}
        // Ignore escape sequence {{ and }} => replaced by {} in the output string
        // Convert the type into string
        // All args and replacement fields must be used

        // LINE: 5394
        return FormatEngine(fmt.string, MakeFormatArgs<Encoding>(args...));
    }

    void Test(int b, int a)
    {
        int l{};
        Format<Utf8Encoding>(u8"{}", l);
        Format<AsciiEncoding>("{}", l);
    }

    constexpr Utf8 S(u8"");

} // namespace SSSEngine::Text
