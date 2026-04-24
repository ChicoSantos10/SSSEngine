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
 * @brief String implementation
 */

#pragma once

#include "AsciiEncoding.h"
#include "Attributes.h"
#include "Debug.h"
#include "Encoding.h"
#include "Types.h"
#include "Bits.h"
#include "Utf8Encoding.h"

namespace SSSEngine::Text
{
    // INVESTIGATE: What file should this be in
    /**
     * @brief Counts how many code units a string contains, not counting the null terminator
     *
     * @param string The string to count
     * @return The number of code units until the null terminator
     */
    template<StringTypeConcept CharType>
    SSSENGINE_PURE SSSENGINE_GLOBAL constexpr Size Length(const CharType *const string) noexcept
    {
        if consteval
        {
            Size index = 0;
            for(; string[index] != CharType('\0'); ++index)
            {
            };

            return index;
        }

        if constexpr(sizeof(CharType) == sizeof(char))
        {
            const auto *data = reinterpret_cast<const char *>(string);
            return __builtin_strlen(data);
        }

        if constexpr(IsSameType<CharType, wchar_t>)
        {
            const auto *data = reinterpret_cast<const wchar_t *>(string);
            return __builtin_wcslen(data);
        }
    }

    /**
     * @brief Represents a lightweight and read-only view into a string
     *
     */
    template<EncodingConcept Encoding>
    class StringView
    {
        using CharType = Encoding::CharType;

        public:
        constexpr StringView(const StringView &literal) = default;
        constexpr StringView(StringView &&literal) noexcept = default;
        constexpr StringView &operator=(const StringView &literal) = default;
        constexpr StringView &operator=(StringView &&) = default;

        constexpr StringView() = delete;

        template<Size N>
        constexpr StringView(const CharType (&data)[N]) : m_data{data}, m_size{N - 1} // NOLINT(*-explicit-constructor)
        {
        }

        constexpr StringView(const CharType *data, const Size size) : m_data{data}, m_size{size} {}

        constexpr StringView(const CharType *data) : // NOLINT(*-explicit-constructor)
        m_data(data), m_size(Length(data))
        {
        }

        ~StringView() = default;

        /**
         * @brief Gets the string
         *
         * @return A pointer to the array of characters
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE constexpr const CharType *Data() const noexcept
        {
            return m_data;
        }

        /**
         * @brief Gets the string as an array of bytes
         *
         * @return The number of code units
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE constexpr const char *RawData() const noexcept
            requires(sizeof(CharType) == sizeof(char))
        {
            return BitCopy<const char *>(m_data);
        }

        /**
         * @brief Counts the number of code units this string literal has
         *
         * @return The number of code units
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE constexpr Size Count() const noexcept
        {
            return m_size;
        }

        private:
        const CharType *m_data;
        Size m_size;
    };

    using Utf8View = StringView<Utf8Encoding>;
    using AsciiView = StringView<AsciiEncoding>;

    SSSENGINE_STATIC_ASSERT(IsTriviallyCopyable<Utf8View>, "StringView must be trivially copyable");
    SSSENGINE_STATIC_ASSERT(IsTriviallyCopyable<AsciiView>, "StringView must be trivially copyable");
} // namespace SSSEngine::Text
