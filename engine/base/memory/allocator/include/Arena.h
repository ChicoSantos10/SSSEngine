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

#include "AllocatorConcept.h"
#include "ApplicationAllocator.h"
#include "Attributes.h"
#include "Buffer.h"
#include "Debug.h"
#include "Math.h"
#include "Types.h"

namespace SSSEngine::Memory
{
    class Arena
    {
      public:
        Arena(const Arena &) = delete;
        Arena(Arena &&) = delete;
        Arena &operator=(const Arena &) = delete;
        Arena &operator=(Arena &&) = delete;

        explicit Arena(Math::Bytes size) : m_buffer(Request(size)) {}

        /**
         * @brief Moves the arena offset and gives back an aligned pointer
         *
         * @param size The size of the allocation
         * @param alignment The alignment of the pointer
         * @return The aligned pointer if the arena has enough space left otherwise nullptr
         */
        void *Allocate(Math::Bytes size, SizeType alignment) noexcept
        {
            auto start = reinterpret_cast<Byte *>(m_buffer.address);
            auto offset = Math::NextMultiplePowerOf2<uintptr>(m_offset, alignment);

            if(offset > m_buffer.capacity || size > m_buffer.capacity - offset)
            {
                // INVESTIGATE: What to do? Grow?
                return nullptr;
            }

            m_offset = offset + size;
            return start + offset;
        }

        /**
         * @brief In Arenas there are no individual frees so this is a no op
         *
         */
        SSSENGINE_FORCE_INLINE
        void Free(Buffer) const noexcept
        {
        }

        /**
         * @brief Resets the arena cleaning all previous allocations
         *
         */
        SSSENGINE_FORCE_INLINE
        void Reset() noexcept
        {
            m_offset = 0;
        }

        ~Arena()
        {
            Memory::Release(m_buffer);
        }

      private:
        Buffer m_buffer;
        uintptr m_offset{0};
    };

    SSSENGINE_STATIC_ASSERT(AllocatorConcept<Arena>, "Arena must be an allocator");
} // namespace SSSEngine::Memory
