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
        using View = StringView<Encoding>;

      public:
        using Iterator = CharType *;
        using ConstIterator = const CharType *;

        constexpr String() noexcept : m_count(0), m_isSmall(true)
        {
            BraceConstructAt<ArrayType>(AddressOf(m_data.stackString));
        }

        explicit constexpr String(View string) : m_count(string.Count())
        {
            SSSENGINE_ASSERT(string.Data());

            if(m_count <= MaxCountSmall)
            {
                CreateSmallString(string);
                m_isSmall = true;
            }
            else
            {
                CreateHeapString(string);
                m_isSmall = false;
            }
        }

        constexpr String(const String &string) : String(View{string}) {}

        template<EncodingConcept E>
        constexpr explicit String(const String<E> &string) :
            m_count(string.m_count), m_isSmall(m_count <= MaxCountSmall)
        {
            // TODO: Implement when conversion between different encodings is implemented
            SSSENGINE_NOT_IMPLEMENTED;
        }

        constexpr String(String &&string) noexcept : m_count(string.m_count), m_isSmall(string.m_isSmall)
        {
            if(m_isSmall)
            {
                CreateSmallString(string);
            }
            else
            {
                m_data.heapString = string.m_data.heapString;

                // INVESTIGATE: Do we make the moved from string, an ordinary stack string by zero init the stack
                //  string?
                string.m_data.heapString = {};
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

            m_isSmall = string.m_isSmall;
            m_count = string.m_count;

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

                // INVESTIGATE: Same doubts as above in move constructor
                string.m_data.heapString = {};
            }

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
                return m_data.stackString.data;
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
        constexpr operator View(this Self &self) noexcept // NOLINT(*-explicit-constructor)
        {
            return {self.CString(), self.Count()};
        }

        constexpr void Reserve(SizeType amount)
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
        constexpr ConstIterator ConstBegin() const noexcept
        {
            return Begin();
        }

        template<typename Self>
        constexpr auto End(this Self &self) noexcept
        {
            return self.Begin() + self.Count();
        }

        SSSENGINE_PURE
        constexpr ConstIterator ConstEnd() const noexcept
        {
            return End();
        }

        // TODO: Reverse Iterators and ranges

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool IsEmpty() const noexcept
        {
            return m_count == 0;
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
            ArrayType stackString;
            SSSENGINE_OVERLAP HeapString heapString;
        };

        SSSENGINE_OVERLAP Data m_data;
        u32 m_count : 31;
        bool m_isSmall : 1;

        /**
         * @brief Starts the lifetime of the small string, copies the data from the string view and fills the rest with
         * 0 bytes
         *
         */
        SSSENGINE_FORCE_INLINE
        constexpr void CreateSmallString(View view)
        {
            m_isSmall = true;

            DefaultConstructAt(AddressOf(m_data.stackString));
            const auto [_, last] = Iterators::Copy(view, m_data.stackString.Begin());
            Iterators::ZeroFill<CharType>(last, m_data.stackString.End());
        }

        /**
         * @brief Copies a small string to this string. This performs a simple memory copy of the bytes of string
         *
         * @param string A string to copy from
         */
        constexpr void CopySmallString(View string)
        {
            SSSENGINE_ASSERT(m_isSmall);
            SSSENGINE_ASSERT(string.Count() <= MaxCountSmall);

            Iterators::Copy(string, m_data.stackString.Begin());
            m_count = string.m_count;
        }

        /**
         * @brief Copies a Heap string to this string
         *
         * @param string The string to copy from
         */
        constexpr void CopyHeapString(View string)
        {
            SSSENGINE_ASSERT(!m_isSmall);
            SSSENGINE_ASSERT(m_data.heapString.m_capacity > string.Count());

            Iterators::Copy(string, m_data.heapString.m_data);
        }

        /**
         * @brief Creates and allocates enough memory for the string.
         *
         * @param size The amount of bytes the string occupies
         * @param capacity The amount of code units that the memory allocated allows for
         * @param count The amount of code units currently in the string
         */
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
         * @brief Creates a Heap string from another string, by first allocating enough memory and then copy from the
         * string to the newly allocated memory
         *
         * @param string The string to create from
         */
        constexpr void CreateHeapString(View string)
        {
            auto count = string.Count();
            auto capacity = count + 1;
            auto bytes = Math::Bytes(capacity * sizeof(CharType));
            CreateHeapString(bytes, capacity, count);

            MemoryCopy(string.Data(), m_data.heapString.m_data, bytes);
            m_data.heapString.m_data[m_count] = CharType(0);
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
