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
#include "Span.h"

namespace SSSEngine::Containers
{
    template<typename T>
    concept SinkConcept = true;

    // template<typename T>
    // concept SinkConcept = requires { T::Iterator; } && requires(T &sink, SizeType n, T::Iterator it) {
    //     { sink.Reserve(n) } -> SameAsConcept<bool>;
    //     { sink.Current() } -> Ranges::IteratorConcept;
    //     { Move(sink).Get() };
    //     { sink.AdvanceTo(it) };
    //     { sink.Advance(n) };
    // };

    template<typename T>
    struct SinkBuffer
    {
        static constexpr SizeType StackSize = Memory::CacheLineConstructive * 5;
        static constexpr SizeType Elements = StackSize / sizeof(T);
        T storage[Elements];
    };

    template<Containers::ContainerConcept T>
    class DirectSink
    {
      public:
        using SinkOutput = T;
        using SinkIterator = Ranges::IteratorType<T>;

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

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        SinkOutput Get() && noexcept
        {
            return Move(m_out);
        }

      private:
        SinkOutput m_out;
    };

} // namespace SSSEngine::Containers
