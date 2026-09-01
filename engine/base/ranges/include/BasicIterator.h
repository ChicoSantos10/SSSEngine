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
#include "Iterator.h"

namespace SSSEngine::Ranges
{

    /**
     * @class BasicIterator
     * @brief Adaptor for iterators that are pure pointer iterators
     *
     * @tparam Iterator The type of iterator
     */
    template<ObjectPointerConcept Iterator>
    class BasicIterator
    {
      public:
        using ValueType = IteratorValueType<Iterator>;
        using ReferenceType = IteratorReferenceType<Iterator>;
        using DifferenceType = IteratorDifferenceType<Iterator>;

        constexpr BasicIterator() = default;
        constexpr BasicIterator(const BasicIterator &it) = default;
        constexpr BasicIterator(BasicIterator &&) = default;
        constexpr BasicIterator &operator=(const BasicIterator &) = default;
        constexpr BasicIterator &operator=(BasicIterator &&) = default;
        constexpr ~BasicIterator() = default;

        constexpr explicit BasicIterator(const Iterator &it) : m_it(it) {}

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReferenceType operator*() const noexcept
        {
            return *m_it;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto *operator->() const noexcept
        {
            return m_it;
        }

        SSSENGINE_FORCE_INLINE
        constexpr BasicIterator &operator++() noexcept
        {
            ++m_it;
            return *this;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr BasicIterator operator++(int) noexcept
        {
            return BasicIterator(m_it++);
        }

        SSSENGINE_FORCE_INLINE
        constexpr BasicIterator &operator--() noexcept
        {
            --m_it;
            return *this;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr BasicIterator operator--(int) noexcept
        {
            return BasicIterator(m_it--);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReferenceType operator[](DifferenceType index) const noexcept
        {
            return m_it[index];
        }

        SSSENGINE_FORCE_INLINE
        constexpr BasicIterator &operator+=(DifferenceType offset) noexcept
        {
            m_it += offset;
            return *this;
        }

        SSSENGINE_FORCE_INLINE
        constexpr BasicIterator &operator-=(DifferenceType offset) noexcept
        {
            m_it -= offset;
            return *this;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator Underlying() const noexcept
        {
            return m_it;
        }

      private:
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr bool operator==(BasicIterator lhs, BasicIterator rhs) noexcept
        {
            return lhs.m_it == rhs.m_it;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr auto operator<=>(BasicIterator lhs, BasicIterator rhs) noexcept
        {
            return lhs.m_it <=> rhs.m_it;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr BasicIterator operator+(BasicIterator lhs, DifferenceType offset) noexcept
        {
            return BasicIterator(lhs.m_it + offset);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr BasicIterator operator+(DifferenceType offset, BasicIterator lhs) noexcept
        {
            return BasicIterator(lhs.m_it + offset);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr DifferenceType operator-(BasicIterator lhs, BasicIterator rhs) noexcept
        {
            return lhs.m_it - rhs.m_it;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr BasicIterator operator-(BasicIterator lhs, DifferenceType offset) noexcept
        {
            return BasicIterator(lhs.m_it - offset);
        }

        Iterator m_it{};
    };

} // namespace SSSEngine::Ranges
