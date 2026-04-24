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

#include "Allocator.h"
#include "AsciiEncoding.h"
#include "Attributes.h"
#include "Bits.h"
#include "Debug.h"
#include "Encoding.h"
#include "Limits.h"
#include "MemoryUtility.h"
#include "StringView.h"
#include "Types.h"
#include "Utf8Encoding.h"
#include "Endian.h"
#include "Utility.h"

namespace SSSEngine::Text
{

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
        using CharType = Encoding::CharType;

        public:
        constexpr String() noexcept
        {
            ZeroMemory(&stackString, sizeof(*this));
            SetCount(0);
        }

        explicit constexpr String(StringView<Encoding> string)
        {
            SSSENGINE_ASSERT(string.Data());

            u32 codeUnits = static_cast<u32>(string.Count());

            if(codeUnits < MaxSmallSize)
            {
                u32 bytes = codeUnits * sizeof(CharType);
                MemoryCopy(string.Data(), stackString, codeUnits * sizeof(CharType));

                u32 leftover = sizeof(HeapString) - bytes;
                ZeroMemory(stackString + bytes + 1, leftover);
                SetSmallStringCount(codeUnits);

                SSSENGINE_ASSERT(IsStackString());
            }
            else
            {
                u32 bytes = (codeUnits + 1) * sizeof(CharType);
                CreateHeapString(Math::Bytes(bytes), codeUnits + 1, CategoryMask);
                SetHeapCount(codeUnits);
                MemoryCopy(string.Data(), heapString.data, bytes);

                SSSENGINE_ASSERT(!IsStackString());
            }
        }

        constexpr String(const String<Encoding> &string)
        {
            if(string.IsStackString())
            {
                CopySmallString(string);
            }
            else
            {
                CopyHeapString(string);
            }
        }

        template<EncodingConcept E>
        constexpr explicit String(const String<E> &string)
        {
            // TODO: Implement when conversion between different encodings is implemented
            SSSENGINE_NOT_IMPLEMENTED;
        }

        constexpr String(String &&string) noexcept
        {
            if(string.IsStackString())
            {
                CopySmallString(string);
            }
            else
            {
                heapString = string.heapString;
            }
            ZeroMemory(&string, sizeof(string));
        }

        constexpr String &operator=(const String &string) noexcept
        {
            if(!IsStackString())
            {
                DeleteHeapString();
            }

            if(string.IsStackString())
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
            if(!IsStackString())
            {
                DeleteHeapString();
            }

            if(string.IsStackString())
            {
                CopySmallString(string);
            }
            else
            {
                heapString = string.heapString;
            }
            ZeroMemory(&string, sizeof(string));

            return *this;
        }

        constexpr ~String()
        {
            if(!IsStackString())
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
            if(IsStackString())
            {
                return stackString;
            }
            return heapString.data;
        }

        /**
         * @brief Gets a null terminated string
         *
         * @return A pointer to a null terminated string
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr const CharType *CString() const noexcept
        {
            if(IsStackString())
            {
                return stackString;
            }
            return heapString.data;
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
            if(IsStackString())
            {
                return SmallStringCount();
            }

            return HeapStringCount();
        }

        constexpr operator StringView<Encoding>(this auto &&self) // NOLINT(*-explicit-constructor)
        {
            return {Forward(self).CString(), Forward(self).Count()};
        }

        private:
        struct HeapString
        {
            CharType *data;
            u32 capacity;
            u32 count;
        };

        SSSENGINE_STATIC_ASSERT(sizeof(HeapString) == 16, "Heap string must contain 16 bytes for proper alignment");
        SSSENGINE_STATIC_ASSERT(alignof(HeapString) == 8, "Heap string must be 8 byte aligned");

        static constexpr u32 MaxSmallSize = sizeof(HeapString) / sizeof(CharType);
        static constexpr u32 MaxCountSmall = MaxSmallSize - 1;
        static constexpr u32 MaxCountBig = 2147483648;
        static constexpr u32 CategoryMask = System::IsLittleEndian() ? 1u << (Math::Limits::Bits<u32> - 1) : 1u;
        static constexpr u8 CategoryByteMask = System::IsLittleEndian() ? 1u << (Math::Limits::Bits<u8> - 1) : 1u;

        union
        {
            CharType stackString[MaxSmallSize];
            HeapString heapString;
        };

        /**
         * @brief Checks if this string is a stack allocated string or a heap allocated string
         *
         * @return True if is stack allocated string, false if is a heap allocated string
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        bool IsStackString() const noexcept
        {
            u8 lastByte = LastByte();

            return !HasBitSet(lastByte, CategoryByteMask);
        }

        /**
         * @brief Sets the count of the string depending on it's category
         *
         * @param count The new string count
         */
        void SetCount(u32 count) noexcept
        {
            if(IsStackString())
            {
                SetSmallStringCount(count);
            }
            else
            {
                SetHeapCount(count);
            }
        }

        /**
         * @brief Sets the count for a small string
         *
         * @param size The new count
         */
        void SetSmallStringCount(u32 count) noexcept
        {
            SSSENGINE_ASSERT(IsStackString());
            SSSENGINE_ASSERT(count <= MaxCountSmall);

            constexpr auto Shift = System::IsLittleEndian() ? 0 : 1;
            stackString[MaxCountSmall] = u8((MaxCountSmall - count) << Shift);
            stackString[count] = '\0';

            SSSENGINE_ASSERT(IsStackString() && Count() == count);
        }

        /**
         * @brief Sets the count for a Heap string
         *
         * @param size The new count for the heap string
         */
        SSSENGINE_FORCE_INLINE
        void SetHeapCount(u32 count) noexcept
        {
            SSSENGINE_ASSERT(!IsStackString());
            SSSENGINE_ASSERT(count < MaxCountBig);

            heapString.count |= System::IsLittleEndian() ? count : count << 1;

            SSSENGINE_ASSERT(!IsStackString());
        }

        /**
         * @brief Gets the count for a small string
         *
         * @return The number of code units this string contains
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        u32 SmallStringCount() const noexcept
        {
            constexpr auto Shift = System::IsLittleEndian() ? 0 : 1;
            auto size = static_cast<u32>(stackString[MaxCountSmall]) >> Shift;

            SSSENGINE_ASSERT(MaxCountSmall >= size);
            return MaxCountSmall - size;
        }

        /**
         * @brief Gets the count of a Heap string
         *
         * @return The number of code units this string contains
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        u32 HeapStringCount() const noexcept
        {
            return WithoutBits(heapString.count, CategoryMask);
        }

        /**
         * @brief Gets the byte representation of this object
         *
         * @note This does NOT return the bytes of the string, but the object itself
         *
         * @return An array of bytes
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        u8 *Bytes() const noexcept
        {
            return BitCopy<u8 *>(this);
        }

        /**
         * @brief Gets the last byte of this object internal memory representation
         *
         * @return The last byte value
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        u8 LastByte() const noexcept
        {
            auto bytes = Bytes();
            return bytes[sizeof(HeapString) - 1];
        }

        /**
         * @brief Copies a small string to this string. This performs a simple memory copy of the bytes of string
         *
         * @param string A string to copy from
         */
        void CopySmallString(const String &string)
        {
            SSSENGINE_ASSERT(IsStackString());
            MemoryCopy(string.stackString, stackString, sizeof(string));
        }

        /**
         * @brief Copies a Heap string to this string
         *
         * @param string The string to copy from
         */
        void CopyHeapString(const String &string)
        {
            SSSENGINE_ASSERT(!string.IsStackString());

            u32 count = string.HeapStringCount();
            u32 size = count * sizeof(CharType) + 1;
            CreateHeapString(Math::Bytes(size), size, string.heapString.count);
            MemoryCopy(string.heapString.data, heapString.data, size);
        }

        void CreateHeapString(Math::Bytes size, u32 capacity, u32 count)
        {
            heapString = {.data = reinterpret_cast<CharType *>(Memory::CurrentAllocator().Allocate(size, alignof(CharType))),
                          .capacity = capacity,
                          .count = count};
        }

        /**
         * @brief Deallocates the Heap string memory
         */
        void DeleteHeapString()
        {
            SSSENGINE_ASSERT(!IsStackString());
            Memory::CurrentAllocator().Free({.address = heapString.data, .capacity = {heapString.capacity}});
        }
    };

    using Utf8 = String<Utf8Encoding>;
    using Ascii = String<AsciiEncoding>;

} // namespace SSSEngine::Text
