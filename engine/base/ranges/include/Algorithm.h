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

#include "Address.h"
#include "Attributes.h"
#include "Concepts.h"
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "InitializerList.h"
#include "Iterator.h"
#include "MemoryUtility.h"
#include "Range.h"
#include "Utility.h"

namespace SSSEngine::Ranges
{
    // TODO: Add noexcept where applicable and proper iterator and range concepts

    template<IteratorConcept It>
        requires OrderableConcept<IteratorValueType<It>>
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
        requires OrderableConcept<IteratorValueType<It>>
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
        requires OrderableConcept<T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto Max(InitializerList<T> list)
    {
        return *FindMaxElement(list);
    }

    template<IteratorConcept It>
        requires OrderableConcept<IteratorValueType<It>>
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
        requires OrderableConcept<IteratorValueType<It>>
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

    template<OrderableConcept T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto Min(InitializerList<T> list)
    {
        return *FindMinElement(list);
    }

    template<IteratorConcept In, IteratorConcept Out>
    struct CopyResult
    {
        In in;
        Out out;
    };

    template<InputIteratorConcept Begin, SentinelForConcept<Begin> End, OutputIteratorConcept<IteratorValueType<Begin>> To>
        requires SameAsConcept<IteratorValueType<Begin>, IteratorValueType<To>>
    constexpr CopyResult<Begin, To> Copy(Begin start, End end, To to) noexcept
    {
        using Type = IteratorValueType<Begin>;
        static constexpr bool AreContiguous = ContiguousMemoryIteratorConcept<Begin> && ContiguousMemoryIteratorConcept<To>;
        static constexpr bool TriviallyCopyable = IsTriviallyCopyable<Type>;

        if constexpr(AreContiguous && TriviallyCopyable)
        {
            SizeType count = end - start;
            SizeType amount = count * sizeof(Type);
            RawMemoryCopy(ToAddress(start), ToAddress(to), amount);

            return {end, to + count};
        }
        else
        {
            // TODO: Optimize with SIMD
            while(start != end)
            {
                *to++ = *start++;
            }

            return {start, to};
        }
    }

    template<BorrowedRangeConcept Range, OutputIteratorConcept<RangeValueType<Range>> To>
    constexpr CopyResult<IteratorType<Range>, To> Copy(Range &&range, To to) noexcept
    {
        return Copy(Begin(range), End(range), to);
    }

    // TODO: Copy for InitializerList? and Ranges. noexcept if applicable

    // TODO: Move

    template<InputIteratorConcept It, SentinelForConcept<It> Sentinel>
    constexpr It Find(It begin, Sentinel end, const IteratorValueType<It> &value) noexcept
    {
        while(begin != end)
        {
            if(*begin == value)
            {
                return begin;
            }
            ++begin;
        }

        return begin;
    }

    template<BorrowedRangeConcept Range>
    constexpr IteratorType<Range> Find(const Range &&range, const RangeValueType<Range> &value) noexcept
    {
        return Find(Begin(range), End(range), value);
    }

    template<InputIteratorConcept It, SentinelForConcept<It> Sentinel>
    constexpr SizeType FindIndex(It begin, Sentinel end, const IteratorValueType<It> &value) noexcept
    {
        SizeType count = 0;
        while(begin != end)
        {
            if(*begin++ == value)
            {
                return count;
            }
            ++count;
        }

        return count;
    }

    template<BorrowedRangeConcept Range>
    constexpr SizeType FindIndex(const Range &&range, const RangeValueType<Range> &value) noexcept
    {
        return FindIndex(Begin(range), End(range), value);
    }

    template<typename T, OutputIteratorConcept<T> Out, SentinelForConcept<Out> End>
    constexpr void Fill(Out begin, End end, const T &value) noexcept
    {
        while(begin != end)
        {
            *begin++ = value;
        }
    }

    template<typename T, BorrowedRangeConcept Range>
    constexpr void Fill(Range &&range, const T &value) noexcept
    {
        Fill(Begin(range), End(range), value);
    }

    template<typename T, OutputIteratorConcept<T> Out, SentinelForConcept<Out> End>
        requires requires { T{0}; }
    constexpr void ZeroFill(Out begin, End end) noexcept(noexcept(T{0}))
    {
        Fill(begin, end, T{0});
    }

    template<typename T, BorrowedRangeConcept Range>
        requires requires { T{0}; }
    constexpr void ZeroFill(Range &&range) noexcept(noexcept(T{0}))
    {
        Fill(Begin(range), End(range), T{0});
    }

    template<InputIteratorConcept T, InputIteratorConcept U, SentinelForConcept<T> End>
        requires EqualityComparableWithConcept<IteratorValueType<T>, IteratorValueType<U>>
    constexpr bool Equals(T begin, End end, U other) noexcept
    {
        for(; begin != end; ++begin, ++other)
        {
            if(*begin != *other)
            {
                return false;
            }
        }
    }

    template<RangeConcept R1, RangeConcept R2>
        requires EqualityComparableWithConcept<RangeValueType<R1>, RangeValueType<R2>>
    constexpr bool Equals(R1 &&first, R2 &&other) noexcept
    {
        if constexpr(ContiguousRangeConcept<R1> && ContiguousRangeConcept<R2>)
        {
            if(Count(first) != Count(other))
            {
                return false;
            }

            Equals(Begin(first), End(first), Begin(other));
        }
        else
        {
            SSSENGINE_NOT_IMPLEMENTED;
        }
    }

} // namespace SSSEngine::Ranges
