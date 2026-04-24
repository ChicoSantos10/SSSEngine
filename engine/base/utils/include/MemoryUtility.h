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
 * @brief Memory utility functions
 */

#pragma once

#include "Attributes.h"
#include "Bits.h"
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "Types.h"

namespace SSSEngine
{
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    bool PointersOverlap(const void *first, const void *second, Size bytes) noexcept
    {
        auto ptr1 = reinterpret_cast<const byte *>(first);
        auto ptr2 = reinterpret_cast<const byte *>(second);
        return (ptr1 < ptr2 + bytes) && (ptr2 < ptr1 + bytes);
    }

    // FIXME: This functions are not safe. Find a better way to do it by either passing buffers, arrays, or
    //   something where sizes are known!

    /**
     * @brief Copies the bit representation
     *
     * @tparam From Type to copy from
     * @tparam To Type to copy to
     * @param from Memory address to copy from
     * @param to Memory address to copy to
     * @param bytes The amount of bytes to copy
     */
    template<typename From, typename To>
        requires(IsBitwiseCopyable<From> && IsBitwiseCopyable<To>)
    SSSENGINE_FORCE_INLINE
    void MemoryCopy(const From *SSSENGINE_RESTRICT from, To *SSSENGINE_RESTRICT to, Size bytes) noexcept
    {
        SSSENGINE_ASSERT(!PointersOverlap(from, to, bytes));

#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
        __builtin_memcpy(to, from, bytes);
#endif // SSSENGINE_MSVC
    }

    /**
     * @brief Copies the bit representation in reverse order
     *
     * @tparam From Type to copy from
     * @tparam To Type to copy to
     * @param from Memory address to copy from
     * @param to Memory address to copy to
     * @param bytes The amount of bytes to copy
     */
    template<typename From, typename To>
        requires(IsBitwiseCopyable<From> && IsBitwiseCopyable<To>)
    SSSENGINE_FORCE_INLINE
    void ReverseMemoryCopy(const From *SSSENGINE_RESTRICT from, To *SSSENGINE_RESTRICT to, Size bytes) noexcept
    {
        SSSENGINE_ASSERT(!PointersOverlap(from, to, bytes));

        auto current = from + bytes;
        auto end = from - 1;
        while(current != end)
        {
            --current;
            *to = *current;
            ++to;
        }
    }

    /**
     * @brief Sets the memory to a value
     *
     * @tparam T The Type to set to
     * @param to Memory address where to set the value
     * @param value The value to set each byte
     * @param bytes The amount of bytes to set
     */
    template<typename T>
        requires(IsBitwiseCopyable<T>)
    SSSENGINE_FORCE_INLINE
    void MemorySet(T *to, i8 value, Size bytes) noexcept
    {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
        __builtin_memset(to, value, bytes);
#endif // SSSENGINE_MSVC
    }

    /**
     * @brief Zeros the memory from address to address + size
     *
     * @tparam T The Type to zero the memory from
     * @param to Memory address to zero the memory
     * @param bytes The amount of bytes to zero
     */
    template<typename T>
        requires(IsBitwiseCopyable<T>)
    SSSENGINE_FORCE_INLINE
    void ZeroMemory(T *to, Size bytes) noexcept
    {
        MemorySet(to, 0, bytes);
    }

    /**
     * @brief Compares the memory bitwise
     *
     * @tparam From The first Type to compare
     * @tparam To The second Type to compare
     * @param from First memory address
     * @param to Second memory address
     * @param bytes Amount of bytes to compare
     * @return -1 if first is less than second, 0 if the same, 1 if second is greater than first
     */
    template<typename From, typename To>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    int MemoryCompare(const From *from, const To *to, Size bytes) noexcept
    {
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG || SSSENGINE_GCC
        return __builtin_memcmp(to, from, bytes);
#endif // SSSENGINE_MSVC
    }

    /**
     * @brief Compares the memory bitwise
     *
     * @tparam From The first Type to compare
     * @param from First memory address
     * @param compareValue Value to compare
     * @param bytes Amount of bytes to compare
     * @return -1 if first is less than compare value, 0 if the same, 1 if second is greater than first
     */
    template<typename From>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    int MemoryCompare(const From *from, const u8 compareValue, Size bytes) noexcept
    {
        // TODO: SIMD instructions?
        u8 *fromBytes = BitCopy<u8 *>(from);
        for(Size i = 0; i < bytes; ++i, ++fromBytes)
        {
            if(*fromBytes < compareValue)
                return -1;
            if(*fromBytes > compareValue)
                return 1;
        }
        return 0;
    }
} // namespace SSSEngine
