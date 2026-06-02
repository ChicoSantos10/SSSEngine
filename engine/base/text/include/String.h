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

#pragma once

/**
 * @file
 * @brief String
 */

#include "Address.h"
#include "Algorithm.h"
#include "Allocator.h"
#include "AsciiEncoding.h"
#include "Attributes.h"
#include "Debug.h"
#include "Encoding.h"
#include "Math.h"
#include "MemorySize.h"
#include "MemoryUtility.h"
#include "QualifierTraits.h"
#include "StringView.h"
#include "Traits.h"
#include "Types.h"
#include "Utf8Encoding.h"
#include "Utility.h"
#include "Array.h"

namespace SSSEngine::Text
{
    // TODO: Move this to a different file
    // INVESTIGATE: Should this be a struct or free functions? If struct probably
    struct GrowthStrategy
    {
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        static constexpr SizeType NextCapacity(SizeType current, SizeType hint) noexcept
        {
            return Math::Max(NextCapacity(current), hint);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        static constexpr SizeType NextCapacity(SizeType current) noexcept
        {
            return current + (current >> 1);
        }
    };

    /**
     * @class String
     * @brief
     *
     * If string is small it will be allocated on the stack,
     * otherwise it will be allocated on the heap
     *
     */
    template<EncodingConcept Encoding>
    class String
    {
        using CharType = Encoding::CodeUnitType;

      public:
        using Iterator = CharType *;
        using ConstIterator = const CharType *;

        constexpr String() noexcept : m_data(), m_count(0), m_isSmall(true)
        {
            BraceConstructAt<ArrayType>(AddressOf(m_data.stackString));
        }

        explicit constexpr String(StringView<Encoding> string) : m_data(), m_count(string.Count())
        {
            SSSENGINE_ASSERT(string.Data());

            u32 count = static_cast<u32>(string.Count());

            if(count <= MaxCountSmall)
            {
                CreateSmallString();

                u32 bytes = count * sizeof(CharType);
                MemoryCopy(string.Data(), m_data.stackString.data, bytes);

                auto begin = m_data.stackString.Begin() + bytes;
                auto end = m_data.stackString.End();
                Iterators::Fill(begin, end, CharType(0));

                m_isSmall = true;
            }
            else
            {
                u32 bytes = (count + 1) * sizeof(CharType);

                CreateHeapString(Math::Bytes(bytes), count + 1, count);
                MemoryCopy(string.Data(), m_data.heapString.m_data, bytes);
                m_data.heapString.m_data[count] = CharType(0);

                SSSENGINE_ASSERT(!m_isSmall);
            }
        }

        constexpr String(const String<Encoding> &string) :
            m_data(), m_count(string.m_count), m_isSmall(string.m_isSmall)
        {
            if(string.m_isSmall)
            {
                CreateSmallString();
                CopySmallString(string);
            }
            else
            {
                CopyHeapString(string);
            }
        }

        template<EncodingConcept E>
        constexpr explicit String(const String<E> &string) :
            m_data(), m_count(string.m_count), m_isSmall(m_count <= MaxCountSmall)
        {
            // TODO: Implement when conversion between different encodings is implemented
            SSSENGINE_NOT_IMPLEMENTED;
        }

        constexpr String(String &&string) noexcept : m_data(), m_count(string.m_count), m_isSmall(string.m_isSmall)
        {
            if(m_isSmall)
            {
                CopySmallString(string);
            }
            else
            {
                m_data.heapString = string.m_data.heapString;

                string.m_data.heapString = nullptr;
                string.m_data.m_count = 0;
            }
        }

        constexpr String &operator=(const String &string) noexcept
        {
            if(this == &string)
            {
                return *this;
            }

            if(!m_isSmall)
            {
                DeleteHeapString();
            }

            if(string.m_isSmall)
            {
                CopySmallString(string);
            }
            else
            {
                CopyHeapString(string);
            }

            return *this;
        }

        constexpr String &operator=(String &&string) noexcept
        {
            if(this == &string)
            {
                return *this;
            }

            if(!m_isSmall)
            {
                DeleteHeapString();
            }

            if(string.m_isSmall)
            {
                CopySmallString(string);
            }
            else
            {
                m_data.heapString = string.m_data.heapString;
            }
            ZeroMemory(&string, sizeof(string));

            return *this;
        }

        constexpr ~String()
        {
            if(!m_isSmall)
            {
                DeleteHeapString();
            }
        }

        /**
         * @brief Gets a null terminated string
         *
         * @return A pointer to a null terminated string
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr CharType *CString() noexcept
        {
            if(m_isSmall)
            {
                return m_data.stackString;
            }
            return m_data.heapString.m_data;
        }

        /**
         * @brief Gets a null terminated string
         *
         * @return A pointer to a null terminated string
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr const CharType *CString() const noexcept
        {
            if(m_isSmall)
            {
                return m_data.stackString;
            }
            return m_data.heapString.m_data;
        }

        /**
         * @brief Get's the amount of code units the string contains
         *
         * Note that for UTF-8 or UTF-16 encodings, this does not necessarily correspond to the amount of characters
         * since each code point is of variable length
         *
         * @return The amount of code units
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr u32 Count() const noexcept
        {
            return m_count;
        }

        /**
         * @brief Get's the total amount of code units this string can hold
         *
         * Note that for UTF-8 or UTF-16 encodings, this does not necessarily correspond to the amount of characters
         * since each code point is of variable length
         *
         * @return The amount of code units
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr u32 Capacity() const noexcept
        {
            if(m_isSmall)
            {
                return MaxSmallSize;
            }

            return m_data.heapString.m_capacity;
        }

        template<typename Self>
        constexpr operator StringView<Encoding>(this Self &self) noexcept // NOLINT(*-explicit-constructor)
        {
            return {self.CString(), self.Count()};
        }

        void Reserve(SizeType amount)
        {
            SSSENGINE_ASSERT(amount < MaxCountBig);

            if(amount <= Capacity())
            {
                return;
            }

            auto newCapacity = GrowthStrategy::NextCapacity(Capacity(), amount);
            void *address = Memory::CurrentAllocator().Allocate({newCapacity * sizeof(CharType)}, alignof(CharType));

            if(!address) SSSENGINE_UNLIKELY
            {
                // INVESTIGATE: What do we do here?
            }

            if(m_isSmall)
            {
                MemoryCopy(m_data.stackString, address, Count());
            }
            else
            {
                MemoryCopy(m_data.heapString.m_data, address, Count());
            }

            SSSENGINE_ASSERT(!m_isSmall);
            m_data.heapString.m_capacity = newCapacity;
        }

        template<typename Self>
        constexpr auto Begin(this Self &self) noexcept
        {
            using It = ConditionalType<IsConst<RemoveReferenceType<Self>>, ConstIterator, Iterator>;

            if(self.m_isSmall)
            {
                return It{self.m_data.stackString.Begin()};
            }

            return It{self.m_data.heapString.m_data};
        }

        SSSENGINE_PURE
        ConstIterator ConstBegin() const noexcept
        {
            return Begin();
        }

        template<typename Self>
        auto End(this Self &self) noexcept
        {
            return self.Begin() + self.Count();
        }

        SSSENGINE_PURE
        ConstIterator ConstEnd() const noexcept
        {
            return End();
        }

        // TODO: Reverse Iterators and ranges

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        bool IsEmpty() const noexcept
        {
            return Count() > 0;
        }

      private:
        // NOTE: In order to have m_other overlap with the padding from this class, its members have to be private
        class HeapString
        {
            friend String;

            CharType *m_data;
            u32 m_capacity;
        };

        SSSENGINE_STATIC_ASSERT(sizeof(HeapString) == 16, "Heap string must contain 16 bytes for proper alignment");
        SSSENGINE_STATIC_ASSERT(alignof(HeapString) == 8, "Heap string must be 8 byte aligned");

        static constexpr u32 MaxSmallSize = 12 / sizeof(CharType);
        static constexpr u32 MaxCountSmall = (MaxSmallSize - 1);
        static constexpr u32 MaxCountBig = 2147483648;

        using ArrayType = Containers::Array<CharType, MaxCountSmall>;

        union Data
        {
            char dummy;

            ArrayType stackString;
            SSSENGINE_OVERLAP HeapString heapString;
        };

        SSSENGINE_OVERLAP Data m_data;
        u32 m_count : 31;
        bool m_isSmall : 1;

        /**
         * @brief Creates but does not initialize the small string
         *
         * Essentially starts the lifetime of the small string without constructing the chars underneath
         *
         */
        SSSENGINE_FORCE_INLINE
        constexpr void CreateSmallString()
        {
            DefaultConstructAt(AddressOf(m_data.stackString));
        }

        /**
         * @brief Copies a small string to this string. This performs a simple memory copy of the bytes of string
         *
         * @param string A string to copy from
         */
        constexpr void CopySmallString(const String &string)
        {
            SSSENGINE_ASSERT(m_isSmall);
            MemoryCopy(string.m_data.stackString.data, m_data.stackString.data, string.m_count * sizeof(CharType));
            m_count = string.m_count;
        }

        /**
         * @brief Copies a Heap string to this string
         *
         * @param string The string to copy from
         */
        constexpr void CopyHeapString(const String &string)
        {
            SSSENGINE_ASSERT(!m_isSmall);

            u32 size = (m_count + 1) * sizeof(CharType);
            CreateHeapString(Math::Bytes(size), size, string.m_count);
            MemoryCopy(string.m_data.heapString.m_data, m_data.heapString.m_data, size);
        }

        constexpr void CreateHeapString(Math::Bytes size, u32 capacity, u32 count)
        {
            m_data.heapString = {};
            m_data.heapString.m_data =
                static_cast<CharType *>(Memory::CurrentAllocator().Allocate(size, alignof(CharType)));
            m_data.heapString.m_capacity = capacity;

            m_count = count;

            m_isSmall = false;
        }

        /**
         * @brief Deallocates the Heap string memory
         */
        constexpr void DeleteHeapString()
        {
            SSSENGINE_ASSERT(!m_isSmall);
            Memory::CurrentAllocator().Free(
                {.address = m_data.heapString.m_data, .capacity = {m_data.heapString.m_capacity}});
        }
    };

    using Utf8 = String<Utf8Encoding>;
    using Ascii = String<AsciiEncoding>;

    SSSENGINE_STATIC_ASSERT(sizeof(Utf8) == 16);
    SSSENGINE_STATIC_ASSERT(sizeof(Ascii) == 16);

} // namespace SSSEngine::Text
