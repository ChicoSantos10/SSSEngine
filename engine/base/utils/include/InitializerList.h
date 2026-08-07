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
#include "Types.h"

// NOLINTBEGIN
namespace std
{
    template<class E>
    class initializer_list
    {
      public:
        using ValueType = E;
        using Reference = E &;
        using ConstReference = const E &;
        using SizeType = SizeType;
        using Iterator = E *;
        using ConstIterator = const E *;

      private:
        ConstIterator m_array;
        SizeType m_count;

        constexpr initializer_list(ConstIterator a, SizeType l) : m_array(a), m_count(l) {}

      public:
        constexpr initializer_list() noexcept : m_array(0), m_count(0) {}

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr SizeType Count() const noexcept
        {
            return m_count;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstIterator Begin() const noexcept
        {
            return m_array;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstIterator End() const noexcept
        {
            return Begin() + Count();
        }

      private:
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr ConstIterator begin(initializer_list<E> list) noexcept
        {
            return list.Begin();
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr ConstIterator end(initializer_list<E> list) noexcept
        {
            return list.End();
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr ConstIterator Count(initializer_list<E> list) noexcept
        {
            return list.Count();
        }
    };

} // namespace std
  // NOLINTEND

namespace SSSEngine
{
    template<typename T>
    using InitializerList = std::initializer_list<T>;

} // namespace SSSEngine
