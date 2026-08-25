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
        constexpr explicit Allocator(T &allocator) : m_allocator(&allocator), m_allocate{&Allocate<T>}, m_free{&Free<T>}
        {
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr void *Allocate(Math::Bytes size, SizeType alignment)
        {
            return m_allocate(m_allocator, size, alignment);
        }

        // TODO: Reallocate

        SSSENGINE_FORCE_INLINE
        constexpr void Free(Buffer buffer)
        {
            if(buffer.address)
            {
                m_free(m_allocator, buffer);
            }
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
    Arena GlobalArena{Math::Bytes(1_GiB)};

    // TODO: Rethink this:
    SSSENGINE_GLOBAL
    constexpr SizeType MaxAllocators = 32;
    SSSENGINE_GLOBAL
    thread_local Allocator Allocators[MaxAllocators] = {Allocator(GlobalArena)};
    SSSENGINE_GLOBAL
    thread_local SizeType Index = 0;

    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr Allocator &CurrentAllocator()
    {
        SSSENGINE_ASSERT(Index < MaxAllocators);
        return Allocators[Index];
    }

    SSSENGINE_FORCE_INLINE
    constexpr void PushAllocator(AllocatorConcept auto &allocator)
    {
        SSSENGINE_ASSERT(Index + 1 < MaxAllocators);
        Allocators[++Index] = Allocator(allocator);
    }

    SSSENGINE_FORCE_INLINE
    constexpr void PopAllocator()
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

        constexpr explicit AllocatorScope(AllocatorConcept auto &allocator)
        {
            PushAllocator(allocator);
        }

        constexpr ~AllocatorScope()
        {
            PopAllocator();
        }
    };

    // NOTE:-------------------------------------------
    //  Wrappers around CurrentAllocator Allocate.
    //  These allow for constexpr memory allocation
    //  with a call to new and delete during consteval
    // ------------------------------------------------

    template<typename T>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr T *Allocate()
    {
        if consteval
        {
            return new T();
        }
        else
        {
            return static_cast<T *>(CurrentAllocator().Allocate(Math::Bytes(sizeof(T)), alignof(T)));
        }
    }

    template<typename T>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr T *Allocate(SizeType count)
    {
        SSSENGINE_ASSERT(count > 0);

        if consteval
        {
            return new T[count];
        }
        else
        {
            return static_cast<T *>(CurrentAllocator().Allocate(Math::Bytes(sizeof(T) * count), alignof(T)));
        }
    }

    template<typename T>
        SSSENGINE_FORCE_INLINE
    constexpr void Free(T *address)
    {
        if consteval
        {
            delete address;
        }
        else
        {
            CurrentAllocator().Free(Buffer{.address = address, .capacity = {sizeof(T)}});
        }
    }

    template<typename T>
        SSSENGINE_FORCE_INLINE
    constexpr void Free(T *address, SizeType count)
    {
        if consteval
        {
            delete[] address;
        }
        else
        {
            CurrentAllocator().Free(Buffer{.address = address, .capacity = Math::Bytes{sizeof(T) * count}});
        }
    }

} // namespace SSSEngine::Memory
