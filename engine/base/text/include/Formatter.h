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

#include "Attributes.h"
#include "Concepts.h"
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "Limits.h"
#include "MemoryUtility.h"
#include "QualifierTraits.h"
#include "SignTraits.h"
#include "String.h"
#include "StringView.h"
#include "Encoding.h"
#include "Types.h"
#include "Address.h"
#include "Utf8Encoding.h"
#include "Utility.h"
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

        operator StringView<Encoding>() // NOLINT(*-explicit-constructor)
        {
            return string;
        }

        StringView<Encoding> string;
    };

    constexpr FormatString<Utf8Encoding> StringFormat(u8"{}");
    constexpr StringView<Utf8Encoding> Utf = StringFormat.string.Data();
    constexpr auto Count = StringFormat.string.Count();

    template<typename T>
    struct Formatter;

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
    };

    struct CustomType
    {
        void *data;
        void (*format)();
    };

    // TODO: What is context? Should be a struct or at least a concept
    template<EncodingConcept Encoding>
    struct FormatArg
    {
        using CharType = Encoding::CharType;

        ArgType type;

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
        } value;
    };

    template<EncodingConcept Encoding>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArg<Encoding> MakeArg(bool value)
    {
        return {.type = ArgType::Bool, .value = {.boolean = value}};
    }

    template<EncodingConcept Encoding>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArg<Encoding> MakeArg(typename Encoding::CharType value)
    {
        return {.type = ArgType::Char, .value = {.character = value}};
    }

    template<EncodingConcept Encoding>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArg<Encoding> MakeArg(StringView<Encoding> value)
    {
        return {.type = ArgType::String, .value = {.string = value}};
    }

    template<EncodingConcept Encoding>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArg<Encoding> MakeArg(i32 value)
    {
        return {.type = ArgType::Int32, .value = {.i32 = value}};
    }

    template<EncodingConcept Encoding>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArg<Encoding> MakeArg(u32 value)
    {
        return {.type = ArgType::UInt32, .value = {.u32 = value}};
    }

    template<EncodingConcept Encoding>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArg<Encoding> MakeArg(i64 value)
    {
        return {.type = ArgType::Int64, .value = {.i64 = value}};
    }

    template<EncodingConcept Encoding>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArg<Encoding> MakeArg(u64 value)
    {
        return {.type = ArgType::UInt64, .value = {.u64 = value}};
    }

    template<EncodingConcept Encoding>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArg<Encoding> MakeArg(f32 value)
    {
        return {.type = ArgType::Float32, .value = {.f32 = value}};
    }

    template<EncodingConcept Encoding>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArg<Encoding> MakeArg(f64 value)
    {
        return {.type = ArgType::Float64, .value = {.f64 = value}};
    }

    template<EncodingConcept Encoding>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArg<Encoding> MakeArg(void *value)
    {
        return {.type = ArgType::Pointer, .value = {.pointer = value}};
    }

    template<EncodingConcept Encoding, typename T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr FormatArg<Encoding> MakeArg(const T &value)
    {
        SSSENGINE_STATIC_ASSERT(IsDefaultConstructible<Formatter<T>>,
                                "No formatter for type T. You must specialize it by Formatter<T>::Format");
        CustomType c{.format = &Formatter<T>::Format, .data = AddressOf(value)};
        return {.type = ArgType::Custom, .value = c};
    }

    template<EncodingConcept Encoding, typename... Args>
    class FormatArgsStorage
    {
        public:
        static constexpr Size ArgCount = sizeof...(Args);
        using Arg = FormatArg<Encoding>;

        explicit constexpr FormatArgsStorage(Args &&...args) : m_args{MakeArg<EncodingConcept>(Forward<Args>(args))...}
        {
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        Arg *Data() const noexcept
        {
            return m_args;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        Size Count() const noexcept
        {
            return ArgCount;
        }

        private:
        // TODO: What would be a reasonable amount before needing to allocate memory?
        Arg m_args[ArgCount];
    };

    template<EncodingConcept Encoding, SignedIntegralConcept T>
    void ParseInt(T value, typename Encoding::CharType *buffer)
    {
        // TODO: How memory allocation will work?

        using CharType = Encoding::CharType;

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

        Size i = 0;
        for(; unsignedValue > 0; ++i)
        {
            tmp[i] = CharType('0') + (unsignedValue % 10);
            unsignedValue /= 10;
        }

        Size out = 0;
        if(isNegative)
        {
            buffer[out++] = CharType('-');
        }

        ReverseMemoryCopy(tmp, &buffer[out], i);
    }

    template<EncodingConcept Encoding, typename... Args>
    auto MakeFormatArgs(Args &&...args)
    {
        char8 buff[20];
        ParseInt<Utf8Encoding>(12, buff);
        return FormatArgsStorage<Encoding, RemoveCVReferenceType<Args>...>(Forward<Args>(args)...);
    }

    template<EncodingConcept Encoding>
    struct FormatArgs
    {
        using Arg = FormatArg<Encoding>;

        template<typename... Args>
        using Storage = FormatArgsStorage<Encoding, Args...>;

        template<typename... Args>
        constexpr FormatArgs(const Storage<Args...> &storage) : // NOLINT(*-explicit-constructor)
        data{storage.Data()}, size{storage.Count()}
        {
        }

        const Arg *data;
        Size size;
    };

    template<EncodingConcept Encoding>
    void FormatEngine(StringView<Encoding> string, FormatArgs<Encoding> args);

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
    Utf8 Format(FormatString<Encoding, IdentityType<Args>...> format, Args &&...args)
    {
        // Find replacement fields: {} which can have an Id and/or a format spec {id:spec}
        // Ignore escape sequence {{ and }} => replaced by {} in the output string
        // Convert the type into string
        // All args and replacement fields must be used

        FormatEngine(format.string, MakeFormatArgs<Encoding>(Forward<Args>(args)...));
    }

    void Test()
    {
        Format<Utf8Encoding>(u8"{}", 1);
    }
} // namespace SSSEngine::Text
