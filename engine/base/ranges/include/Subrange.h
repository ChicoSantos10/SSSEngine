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

#include "ArrayTraits.h"
#include "Attributes.h"
#include "Concepts.h"
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "Iterator.h"
#include "ObjectConcepts.h"
#include "QualifierTraits.h"
#include "Range.h"
#include "SignTraits.h"
#include "Traits.h"
#include "View.h"

namespace SSSEngine::Ranges
{
    template<typename From, typename To>
    concept UsesNonQualificationPointerConcept =
        IsPointer<From> && IsPointer<To> && !IsArrayConvertible<RemovePointerType<From>, RemovePointerType<To>>;

    template<typename From, typename To>
    concept ConvertibleToNonSlicingConcept =
        ConvertibleToConcept<From, To> && !UsesNonQualificationPointerConcept<DecayType<From>, DecayType<To>>;

    enum class SubrangeKind : bool
    {
        Unsized,
        Sized
    };

    template<IteratorConcept Iterator,
             SentinelForConcept<Iterator> Sentinel = Iterator,
             SubrangeKind Kind = SizedSentinelForConcept<Sentinel, Iterator> ? SubrangeKind::Sized : SubrangeKind::Unsized>
        requires(Kind == SubrangeKind::Sized || !SizedSentinelForConcept<Sentinel, Iterator>)
    class Subrange : ViewInterface<Subrange<Iterator, Sentinel, Kind>>
    {
      private:
        static constexpr bool StoreCount = Kind == SubrangeKind::Sized && SizedSentinelForConcept<Sentinel, Iterator>;
        using IteratorDifference = IteratorDifferenceType<Iterator>;
        using CountType = MakeUnsigned<IteratorDifference>;

        template<bool StoreValue>
        struct CountStorage
        {
            explicit CountStorage(CountType) noexcept {}
        };

        template<>
        struct CountStorage<true>
        {
            CountType count{0};
        };

        static consteval bool IsAdvanceNoExcept(IteratorDifference offset) noexcept
        {
            auto it = DeclVal<Iterator>();

            if constexpr(BidirectionalIteratorConcept<Iterator>)
            {
                return noexcept(Ranges::Advance(it, offset));
            }
            else
            {
                auto end = DeclVal<Sentinel>();
                return noexcept(Ranges::Advance(it, offset, end));
            }
        }

      public:
        // Constructors
        constexpr Subrange() noexcept
            requires DefaultInitializableConcept<Iterator>
        = default;

        constexpr Subrange(ConvertibleToNonSlicingConcept<Iterator> auto it,
                           Sentinel sentinel) noexcept(IsNoThrowConstructible<Iterator, decltype(it)> &&
                                                       IsNoThrowConstructible<Sentinel, Sentinel &>)
            requires(!StoreCount)
            : m_iterator(Move(it)), m_sentinel(sentinel)
        {
        }

        constexpr Subrange(ConvertibleToNonSlicingConcept<Iterator> auto it, Sentinel sentinel, CountType n) noexcept(
            IsNoThrowConstructible<Iterator, decltype(it)> && IsNoThrowConstructible<Sentinel, Sentinel &>)
            requires(Kind == SubrangeKind::Sized)
            : m_iterator(Move(it)), m_sentinel(sentinel), m_count(n)
        {
        }

        // NOLINTBEGIN(google-explicit-constructor)

        template<DifferentFromConcept<Subrange> Range>
            requires BorrowedRangeConcept<Range> && ConvertibleToNonSlicingConcept<IteratorType<Range>, Iterator> &&
                     ConvertibleToConcept<SentinelType<Range>, Sentinel>
                     constexpr Subrange(Range &&range) noexcept(noexcept(Subrange(range, Ranges::Count(range))))
                         requires(!StoreCount || SizedRangeConcept<Range>)
            : Subrange(range, Ranges::Count(range))
        {
        }

        // NOLINTEND(google-explicit-constructor)

        template<BorrowedRangeConcept Range>
            requires ConvertibleToNonSlicingConcept<IteratorType<Range>, Iterator> &&
                     ConvertibleToConcept<SentinelType<Range>, Sentinel> &&
                     (Kind == SubrangeKind::Sized)
                     constexpr Subrange(Range &&range, CountType count) noexcept(noexcept(Ranges::Begin(range)) &&
                                                                                 noexcept(Ranges::End(range)))
                         requires(Kind == SubrangeKind::Sized)
            : Subrange(Ranges::Begin(range), Ranges::End(range), count)
        {
        }

        // Iterator Operations

        SSSENGINE_PURE
        constexpr Subrange &Advance(IteratorDifference offset) noexcept(IsAdvanceNoExcept(offset))
        {
            if constexpr(BidirectionalIteratorConcept<Iterator>)
            {
                if(offset < 0)
                {
                    // INVESTIGATE: Since this decreases begin, we should find a way to make sure the iterator is still
                    // valid
                    Ranges::Advance(m_iterator, offset);
                    if constexpr(StoreCount)
                    {
                        m_count.count += ToUnsigned(-offset);
                    }
                    return *this;
                }
            }
            else
            {
                SSSENGINE_ASSERT(offset >= 0);
                auto moved = offset - Ranges::Advance(m_iterator, offset, m_sentinel);
                if constexpr(StoreCount)
                {
                    m_count.count -= ToUnsigned(moved);
                }
                return *this;
            }
        }

        SSSENGINE_PURE
        constexpr Subrange Previous(IteratorDifference offset = 1) const noexcept(Advance(-offset))
        {
            auto tmp = *this;
            tmp.Advance(-offset);
            return tmp;
        }

        SSSENGINE_PURE
        constexpr Subrange Previous(IteratorDifference offset = 1) && noexcept(Advance(-offset))
        {
            Advance(-offset);
            return Move(*this);
        }

        SSSENGINE_PURE
        constexpr Subrange Next(IteratorDifference offset = 1) const noexcept(Advance(offset))
        {
            auto tmp = *this;
            tmp.Advance(offset);
            return tmp;
        }

        SSSENGINE_PURE
        constexpr Subrange Next(IteratorDifference offset = 1) && noexcept(Advance(offset))
        {
            Advance(offset);
            return Move(*this);
        }

        // Observers

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator Begin() const noexcept
            requires CopyableConcept<Iterator>
        {
            return m_iterator;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator Begin() const noexcept
            requires(!CopyableConcept<Iterator>)
        {
            return Move(m_iterator);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Sentinel End() const noexcept
        {
            return m_sentinel;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr CountType Count() const noexcept
        {
            if constexpr(StoreCount)
            {
                return m_count.count;
            }
            else
            {
                return m_sentinel - m_iterator;
            }
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool IsEmpty() const noexcept
        {
            return m_iterator == m_sentinel;
        }

      private:
        Iterator m_iterator;
        SSSENGINE_OVERLAP Sentinel m_sentinel;
        SSSENGINE_OVERLAP CountStorage<StoreCount> m_count;
    };

    // ----------------
    // Deduction Guides
    // ----------------

    template<IteratorConcept It, SentinelForConcept<It> Sent>
    Subrange(It, Sent) -> Subrange<It, Sent>;

    template<IteratorConcept It, SentinelForConcept<It> Sent>
    Subrange(It, Sent, MakeUnsigned<IteratorDifferenceType<It>>) -> Subrange<It, Sent, SubrangeKind::Sized>;

    template<BorrowedRangeConcept Range>
    Subrange(Range &&)
        -> Subrange<IteratorType<Range>,
                    SentinelType<Range>,
                    (SizedRangeConcept<Range> || SizedSentinelForConcept<SentinelType<Range>, IteratorType<Range>>) ?
                        SubrangeKind::Sized :
                        SubrangeKind::Unsized>;

    template<BorrowedRangeConcept Range>
    Subrange(Range &&, MakeUnsigned<RangeDifferenceType<Range>>)
        -> Subrange<IteratorType<Range>, SentinelType<Range>, SubrangeKind::Sized>;

} // namespace SSSEngine::Ranges
