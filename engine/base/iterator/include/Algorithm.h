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
#include "InitializerList.h"
#include "Iterator.h"
#include "Range.h"

namespace SSSEngine::Iterators
{
    // TODO: Add noexcept where applicable

    template<IteratorConcept It>
        requires ComparableConcept<IteratorValueType<It>>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr It FindMaxElement(It begin, It end)
    {
        if(begin == end)
        {
            return begin;
        }

        auto max = begin;

        while(++begin != end)
        {
            if(*begin > *max)
            {
                max = begin;
            }
        }

        return max;
    }

    template<BorrowedRangeConcept Range>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr IteratorType<Range> FindMaxElement(Range &&range)
    {
        return FindMaxElement(Begin(range), End(range));
    }

    template<IteratorConcept It>
        requires ComparableConcept<IteratorValueType<It>>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr IteratorValueType<It> Max(It begin, It end)
    {
        return *FindMaxElement(begin, end);
    }

    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto Max(RangeConcept auto &&range)
    {
        return *FindMaxElement(Begin(range), End(range));
    }

    template<typename T>
        requires ComparableConcept<T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto Max(InitializerList<T> list)
    {
        return *FindMaxElement(list);
    }

    template<IteratorConcept It>
        requires ComparableConcept<IteratorValueType<It>>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr It FindMinElement(It begin, It end)
    {
        if(begin == end)
        {
            return begin;
        }

        auto max = begin;

        while(++begin != end)
        {
            if(*begin < *max)
            {
                max = begin;
            }
        }

        return max;
    }

    template<BorrowedRangeConcept Range>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr IteratorType<Range> FindMinElement(Range &&range)
    {
        return FindMinElement(Begin(range), End(range));
    }

    template<IteratorConcept It>
        requires ComparableConcept<IteratorValueType<It>>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr IteratorValueType<It> Min(It begin, It end)
    {
        return *FindMinElement(begin, end);
    }

    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto Min(RangeConcept auto &&range)
    {
        return *FindMinElement(Begin(range), End(range));
    }

    template<typename T>
        requires ComparableConcept<T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto Min(InitializerList<T> list)
    {
        return *FindMinElement(list);
    }
} // namespace SSSEngine::Iterators
