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

#include "Arena.h"
#include "Attributes.h"
#include "Buffer.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "MemorySize.h"
#include "AllocatorConcept.h"

namespace SSSEngine::Memory
{

    class Allocator
    {
      public:
        Allocator() = default;

        template<AllocatorConcept T>
        explicit Allocator(T &allocator) : m_allocator(&allocator), m_allocate{&Allocate<T>}, m_free{&Free<T>}
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

        template<typename T>
            SSSENGINE_FORCE_INLINE
        static void *Allocate(void *self, Math::Bytes size, SizeType alignment)
        {
            return static_cast<T *>(self)->Allocate(size, alignment);
        }

        template<typename T>
            SSSENGINE_FORCE_INLINE
        static void Free(void *self, Buffer buffer)
        {
            return static_cast<T *>(self)->Free(buffer);
        }
    };

    // TODO: Default Allocator
    SSSENGINE_GLOBAL
    Arena GlobalArena{Math::Bytes(1_GB)};

    // TODO: Rethink this:
    SSSENGINE_GLOBAL
    constexpr SizeType MaxAllocators = 32;
    SSSENGINE_GLOBAL
    thread_local Allocator Allocators[MaxAllocators] = {Allocator(GlobalArena)};
    SSSENGINE_GLOBAL
    thread_local SizeType Index = 0;

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
        Allocators[++Index] = Allocator(allocator);
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
