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
#include "Debug.h"
#include "Encoding.h"
#include "Iterator.h"
#include "Utf8Encoding.h"

#include <compare>

namespace SSSEngine::Text
{
    template<EncodingConcept Encoding>
    class StringIterator
    {
      public:
        using ValueType = Encoding::CodePointType;
        using CharType = Encoding::CodeUnitType;
        using PointerType = CharType *;

        StringIterator() = default;

        explicit StringIterator(PointerType ptr) : m_current(ptr), m_currentSize(Encoding::CharLength(*ptr)) {}

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        ValueType operator*() const noexcept
        {
            // INVESTIGATE: Should we check or just assert it?
            SSSENGINE_ASSERT(Encoding::ValidCodepoint(m_current, m_currentSize));

            return Encoding::Decode(m_current, m_currentSize);
        }

        SSSENGINE_FORCE_INLINE
        StringIterator &operator++() noexcept
        {
            SSSENGINE_ASSERT(m_current && m_currentSize > 0);

            m_current += m_currentSize;
            m_currentSize = Encoding::CharLength(*m_current);
            return *this;
        }

        SSSENGINE_FORCE_INLINE
        StringIterator operator++(int) noexcept
        {
            StringIterator tmp(*this);
            ++(*this);
            return tmp;
        }

      private:
        PointerType m_current{};
        Size m_currentSize{};

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend auto operator<=>(const StringIterator &lhs, const StringIterator &rhs) = default;
    };

    using Utf8Iterator = StringIterator<Utf8Encoding>;

    struct NullTerminatorSentinel
    {
        template<EncodingConcept Encoding>
        friend bool operator==(const NullTerminatorSentinel, const StringIterator<Encoding> &it)
        {
            return *it == typename Encoding::CodePointType(0);
        }
    };

    SSSENGINE_STATIC_ASSERT(Iterators::MultiPassIteratorConcept<Utf8Iterator>);
    SSSENGINE_STATIC_ASSERT(Iterators::SentinelForConcept<NullTerminatorSentinel, Utf8Iterator>);

} // namespace SSSEngine::Text
