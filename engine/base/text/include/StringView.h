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

#include "Algorithm.h"
#include "AsciiEncoding.h"
#include "Attributes.h"
#include "BasicIterator.h"
#include "Concepts.h"
#include "Debug.h"
#include "Encoding.h"
#include "HelperMacros.h"
#include "Range.h"
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
    template<CharTypeConcept CharType>
    SSSENGINE_PURE SSSENGINE_GLOBAL
    constexpr SizeType Length(const CharType *const string) noexcept
    {
        if consteval
        {
            SizeType index = 0;
            for(; string[index] != CharType('\0'); ++index)
            {
            };

            return index;
        }
        else
        {
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
    }

    /**
     * @brief Represents a lightweight and read-only view into a string
     *
     */
    template<EncodingConcept Encoding>
    class StringView
    {
      public:
        using CharType = Encoding::CodeUnitType;
        using Iterator = Ranges::BasicIterator<const CharType *>;

        constexpr StringView(const StringView &literal) = default;
        constexpr StringView(StringView &&literal) noexcept = default;
        constexpr StringView &operator=(const StringView &literal) = default;
        constexpr StringView &operator=(StringView &&) = default;

        constexpr StringView() = delete;

        // NOLINTBEGIN(*-explicit-constructor)

        template<SizeType N>
        constexpr StringView(const CharType (&data)[N]) noexcept : m_data{data}, m_count{N - 1}
        {
        }

        constexpr StringView(const CharType *data, const SizeType size) noexcept : m_data{data}, m_count{size} {}

        constexpr StringView(const CharType *data) noexcept : m_data(data), m_count(Length(data)) {}

        constexpr StringView(const char *data, const SizeType size) noexcept
            requires IsSameType<Encoding, Utf8Encoding>
            : m_data{BitCopy<const char8 *>(data)}, m_count{size}
        {
        }

        template<SizeType N>
            requires IsSameType<Encoding, Utf8Encoding>
        constexpr StringView(const char (&data)[N]) noexcept : StringView(data, N - 1)
        {
        }

        // NOLINTEND(*-explicit-constructor)

        ~StringView() = default;

        /**
         * @brief Gets the string
         *
         * @return A pointer to the array of characters
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr const CharType *Data() const noexcept
        {
            return m_data;
        }

        /**
         * @brief Gets the string as an array of bytes
         *
         * @return The number of code units
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr const char *RawData() const noexcept
            requires(sizeof(CharType) == sizeof(char))
        {
            return BitCopy<const char *>(m_data);
        }

        /**
         * @brief Counts the number of code units this string literal has
         *
         * @return The number of code units
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr SizeType Count() const noexcept
        {
            return m_count;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator Begin() const noexcept
        {
            return Iterator(m_data);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator End() const noexcept
        {
            return Iterator(m_data + m_count);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr SizeType FindIndex(CharType value) const noexcept;

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr const CharType &operator[](SizeType index) const noexcept
        {
            SSSENGINE_ASSERT(index < m_count);

            return m_data[index];
        }

      private:
        const CharType *m_data;
        SizeType m_count;

        // NOLINTBEGIN(readability-identifier-naming)

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr friend Iterator begin(StringView view) noexcept
        {
            return view.Begin();
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr friend Iterator end(StringView view) noexcept
        {
            return view.End();
        }

        // NOLINTEND(readability-identifier-naming)
    };

    using Utf8View = StringView<Utf8Encoding>;
    using AsciiView = StringView<AsciiEncoding>;

    SSSENGINE_STATIC_ASSERT(IsTriviallyCopyable<Utf8View>, "StringView must be trivially copyable");
    SSSENGINE_STATIC_ASSERT(IsTriviallyCopyable<AsciiView>, "StringView must be trivially copyable");

} // namespace SSSEngine::Text

namespace SSSEngine::Ranges
{
    template<Text::EncodingConcept Encoding>
    SSSENGINE_GLOBAL
    constexpr bool EnableBorrowRange<Text::StringView<Encoding>> = true;

} // namespace SSSEngine::Ranges

namespace SSSEngine::Text
{
    template<EncodingConcept Encoding>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr SizeType StringView<Encoding>::FindIndex(typename Encoding::CodeUnitType value) const noexcept
    {
        return Ranges::FindIndex(*this, value);
    }

} // namespace SSSEngine::Text
