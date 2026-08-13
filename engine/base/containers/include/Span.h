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

namespace SSSEngine::Containers
{
    template<typename T>
    struct Span
    {
        using Iterator = T *;
        using ConstIterator = const T *;

        Iterator beginIt;
        Iterator endIt;

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        operator Span<const T>() const noexcept // NOLINT(*-explicit-constructor)
        {
            return {beginIt, endIt};
        }

        friend constexpr Iterator begin(const Span &span)
        {
            return span.beginIt;
        }

        friend constexpr Iterator end(const Span &span)
        {
            return span.endIt;
        }
    };

    template<typename T>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    Span<T>::Iterator Begin(Span<T> span)
    {
        return begin(span);
    }

    template<typename T>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    Span<T>::ConstIterator ConstBegin(Span<T> span)
    {
        return begin(span);
    }

    template<typename T>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    Span<T>::Iterator End(Span<T> span)
    {
        return end(span);
    }

    template<typename T>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    Span<T>::ConstIterator ConstEnd(Span<T> span)
    {
        return end(span);
    }

} // namespace SSSEngine::Containers
