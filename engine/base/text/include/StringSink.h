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

#include "Attributes.h"
#include "Concepts.h"
#include "Debug.h"
#include "Iterator.h"
#include "Memory.h"
#include "Range.h"
#include "Types.h"
#include "Utility.h"
#include "Container.h"

namespace SSSEngine::Text
{
    template<typename T>
    concept SinkConcept = requires(T &sink, SizeType n) {
        { sink.Reserve(n) } -> SameAsConcept<bool>;
        { sink.Current() } -> Iterators::IteratorConcept;
        { Move(sink).Get() };
        { sink.Advance(n) };
    };

    template<typename T>
    struct SinkBuffer
    {
        static constexpr SizeType StackSize = Memory::CacheLineConstructive * 5;
        static constexpr SizeType Elements = StackSize / sizeof(T);
        T storage[Elements];
    };

    template<typename T>
    class BufferedSink
    {
        SSSENGINE_NOT_IMPLEMENTED;

      public:
        using SinkOutput = T;
        using Iterator = T::ValueType *;
        using SinkBuffer = SinkBuffer<T>;

        void Reserve(SizeType elements) noexcept
        {
            SSSENGINE_ASSERT(elements <= SinkBuffer::Elements);

            if(End() - Current() < elements)
            {
                Flush();
            }
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        Iterator Current() noexcept
        {
            return m_current;
        };

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        Iterator Begin() noexcept
        {
            return m_buffer.storage;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        Iterator End() noexcept
        {
            return m_buffer.storage + SinkBuffer::Elements;
        }

        SSSENGINE_FORCE_INLINE
        void Advance(SizeType count) noexcept
        {
            m_current += count;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        T Get() && noexcept
        {
            Flush();
            return Move(m_sequence);
        }

      private:
        SinkOutput m_sequence;
        SinkBuffer m_buffer;
        Iterator m_current;

        void Flush() noexcept
        {
            if(m_current > Begin())
            {
                m_sequence.Append(m_current, End());
            }
            m_current = Begin();
        }
    };

    template<Containers::ContainerConcept T>
    class DirectSink
    {
      public:
        using SinkOutput = T;
        using SinkIterator = Iterators::IteratorType<T>;

        bool Reserve(SizeType amount)
        {
            SizeType cap = m_out.Count() + amount;
            m_out.Reserve(cap);

            return m_out.Capacity() >= amount;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        SinkIterator Current() noexcept
        {
            m_out.End();
        }

        SSSENGINE_FORCE_INLINE
        void Advance(SizeType) const noexcept
        {
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        SinkOutput Get() && noexcept
        {
            return Move(m_out);
        }

      private:
        SinkOutput m_out;
    };

} // namespace SSSEngine::Text
