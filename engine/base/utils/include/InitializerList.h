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
        using value_type = E;
        using reference = E &;
        using const_reference = E &;
        using size_type = Size;
        using iterator = E *;
        using const_iterator = E *;

        private:
        iterator m_array;
        size_type m_length;

        // The compiler can call a private constructor.
        constexpr initializer_list(const_iterator a, size_type l) : m_array(a), m_length(l) {}

        public:
        constexpr initializer_list() noexcept : m_array(0), m_length(0) {}

        // Number of elements.
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr size_type Size() const noexcept
        {
            return m_length;
        }

        // First element.
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr const_iterator Begin() const noexcept
        {
            return m_array;
        }

        // One past the last element.
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr const_iterator End() const noexcept
        {
            return Begin() + Size();
        }
    };

} // namespace std
  // NOLINTEND

namespace SSSEngine
{
    template<typename T>
    using InitializerList = std::initializer_list<T>;

    constexpr InitializerList<int> V = {1, 2, 3};
} // namespace SSSEngine
