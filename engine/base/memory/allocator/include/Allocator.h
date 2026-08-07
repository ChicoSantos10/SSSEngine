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
 * @brief Type Erased Allocator
 */

#pragma once

#include "Attributes.h"
#include "Buffer.h"
#include "Concepts.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "MemorySize.h"

namespace SSSEngine::Memory
{

    template<typename T>
    concept AllocatorConcept = requires(T allocator, Math::Bytes size, SizeType align, Buffer buffer) {
        { allocator.Allocate(size, align) } -> ConvertibleToConcept<void *>;
        { allocator.Free(buffer) } -> SameAsConcept<void>;
    };

    class Allocator
    {
      public:
        Allocator() = default;

        template<AllocatorConcept T>
        explicit Allocator(T allocator) :
            m_allocator(&allocator), m_allocate{&allocator.Allocate}, m_free{&allocator.Free}
        {
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        void *Allocate(Math::Bytes size, SizeType alignment)
        {
            return m_allocate(m_allocator, size, alignment);
        }

        // TODO: Reallocate

        SSSENGINE_FORCE_INLINE
        void Free(Buffer buffer)
        {
            return m_free(m_allocator, buffer);
        }

      private:
        using AllocateFn = void *(*)(void *, Math::Bytes, SizeType);
        using FreeFn = void (*)(void *, Buffer);

        void *m_allocator{};
        AllocateFn m_allocate{};
        FreeFn m_free{};
    };

    // TODO: Rethink this:
    SSSENGINE_GLOBAL
    constexpr SizeType MaxAllocators = 32;
    SSSENGINE_GLOBAL
    thread_local Allocator Allocators[MaxAllocators];
    SSSENGINE_GLOBAL
    thread_local SizeType Index = -1;

    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    Allocator &CurrentAllocator()
    {
        SSSENGINE_ASSERT(Index >= 0 && Index < MaxAllocators);
        return Allocators[Index];
    }

    SSSENGINE_FORCE_INLINE
    void PushAllocator(AllocatorConcept auto &allocator)
    {
        SSSENGINE_ASSERT(Index + 1 < MaxAllocators);
        Allocators[++Index] = allocator;
    }

    SSSENGINE_FORCE_INLINE
    void PopAllocator()
    {
        SSSENGINE_ASSERT(Index > 0);
        --Index;
    }

    struct AllocatorScope
    {
        AllocatorScope(const AllocatorScope &) = delete;
        AllocatorScope(AllocatorScope &&) = delete;
        AllocatorScope &operator=(const AllocatorScope &) = delete;
        AllocatorScope &operator=(AllocatorScope &&) = delete;

        explicit AllocatorScope(AllocatorConcept auto &allocator)
        {
            PushAllocator(allocator);
        }

        ~AllocatorScope()
        {
            PopAllocator();
        }
    };

} // namespace SSSEngine::Memory
