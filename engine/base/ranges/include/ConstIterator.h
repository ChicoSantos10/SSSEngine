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
#include "ConversionTraits.h"
#include "CopyAndMoveTraits.h"
#include "HelperMacros.h"
#include "Iterator.h"
#include "ObjectConcepts.h"
#include "Ordering.h"
#include "Traits.h"
#include "Utility.h"

namespace SSSEngine::Ranges
{
    template<InputIteratorConcept Iterator>
    class BasicConstIterator;

    // NOLINTBEGIN(readability-identifier-naming, bugprone-reserved-identifier)
    namespace __impl
    {
        template<typename T>
        SSSENGINE_GLOBAL
        constexpr bool __IsBasicConstIterator = false;

        template<typename T>
        SSSENGINE_GLOBAL
        constexpr bool __IsBasicConstIterator<BasicConstIterator<T>> = true;

        template<typename It>
        concept __NotBasicConstIterator = !__IsBasicConstIterator<It>;

    } // namespace __impl

    // NOLINTEND(readability-identifier-naming, bugprone-reserved-identifier)

    template<IndirectlyReadableConcept It>
    using IteratorConstReferenceType = CommonReferenceType<const IteratorValueType<It> &&, IteratorReferenceType<It>>;

    template<typename It>
    concept ConstIteratorConcept =
        InputIteratorConcept<It> && SameAsConcept<IteratorConstReferenceType<It>, IteratorReferenceType<It>>;

    template<IndirectlyReadableConcept It>
    using IteratorConstRValueReference =
        CommonReferenceType<const IteratorValueType<It> &&, IteratorRValueReferenceType<It>>;

    template<InputIteratorConcept Iterator>
    class BasicConstIterator
    {
        template<InputIteratorConcept It>
        friend class BasicConstIterator;

      public:
        using ValueType = IteratorValueType<Iterator>;
        using DifferenceType = IteratorDifferenceType<Iterator>;
        using ReferenceType = IteratorConstReferenceType<Iterator>;
        using RValueReference = IteratorConstRValueReference<Iterator>;

        BasicConstIterator()
            requires DefaultInitializableConcept<Iterator>
        = default;

        constexpr explicit BasicConstIterator(Iterator it) noexcept(IsNoThrowMoveConstructible<Iterator>) :
            m_current(Move(it))
        {
        }

        template<ConvertibleToConcept<Iterator> It>
        constexpr explicit BasicConstIterator(BasicConstIterator<It> it) noexcept(IsNoThrowConstructible<Iterator, It>) :
            m_current(Move(it.m_current))
        {
        }

        template<DifferentFromConcept<Iterator> It>
            requires ConvertibleToConcept<It, Iterator>
        constexpr explicit BasicConstIterator(It &&it) noexcept(IsNoThrowConstructible<Iterator, It>) :
            m_current(Forward<It>(it))
        {
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr const Iterator &Underlying() const & noexcept
        {
            return m_current;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator Underlying() && noexcept(IsNoThrowMoveConstructible<Iterator>)
        {
            return Move(m_current);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReferenceType operator*() const noexcept(noexcept(static_cast<ReferenceType>(*m_current)))
        {
            return static_cast<ReferenceType>(*m_current);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr const auto *operator->() const
            noexcept(ContiguousMemoryIteratorConcept<Iterator> || noexcept(*m_current))
            requires IsLValueReference<IteratorReferenceType<Iterator>>
        {
            if constexpr(ContiguousMemoryIteratorConcept<Iterator>)
            {
                return ToAddress(m_current);
            }
            else
            {
                return AddressOf(*m_current);
            }
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr BasicConstIterator &operator++() noexcept(noexcept(++m_current))
        {
            ++m_current;
            return *this;
        }

        SSSENGINE_FORCE_INLINE
        constexpr void operator++(int) noexcept(noexcept(++m_current))
        {
            ++m_current;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr BasicConstIterator
        operator++(int) noexcept(noexcept(++m_current) && IsNoThrowCopyConstructible<BasicConstIterator>)
            requires MultiPassIteratorConcept<Iterator>
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr BasicConstIterator &operator--() noexcept(noexcept(--m_current))
            requires BidirectionalIteratorConcept<Iterator>
        {
            --m_current;
            return *this;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr BasicConstIterator
        operator--(int) noexcept(noexcept(--m_current) && IsNoThrowCopyConstructible<BasicConstIterator>)
            requires BidirectionalIteratorConcept<Iterator>
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr BasicConstIterator &operator+=(DifferenceType offset) noexcept(noexcept(m_current += offset))
            requires RandomAccessIteratorConcept<Iterator>
        {
            m_current += offset;
            return *this;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr BasicConstIterator &operator-=(DifferenceType offset) noexcept(noexcept(m_current -= offset))
            requires RandomAccessIteratorConcept<Iterator>
        {
            m_current -= offset;
            return *this;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReferenceType operator[](DifferenceType index) const
            noexcept(noexcept(static_cast<ReferenceType>(m_current[index])))
            requires RandomAccessIteratorConcept<Iterator>
        {
            return static_cast<ReferenceType>(m_current[index]);
        }

        template<SentinelForConcept<Iterator> Sentinel>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool operator==(Sentinel sentinel) const noexcept(noexcept(m_current == sentinel))
        {
            return m_current == sentinel;
        }

        // NOLINTBEGIN(google-explicit-constructor)

        template<__impl::__NotBasicConstIterator It>
            requires ConstIteratorConcept<It> && ConvertibleToConcept<Iterator, It>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr operator It() const & noexcept(IsNoThrowConvertible<It, Iterator>)
        {
            return m_current;
        }

        template<__impl::__NotBasicConstIterator It>
            requires ConstIteratorConcept<It> && ConvertibleToConcept<Iterator, It>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr operator It() && noexcept(IsNoThrowConvertible<It, Iterator>)
        {
            return Move(m_current);
        }

        // NOLINTEND(google-explicit-constructor)

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr bool
        operator<=>(const BasicConstIterator &rhs,
                    const BasicConstIterator &lhs) noexcept(noexcept(rhs.m_current <=> lhs.m_current))
            requires ThreeWayComparableConcept<Iterator>
        {
            return rhs.m_current <=> lhs.m_current;
        }

        template<DifferentFromConcept<BasicConstIterator> It>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr bool
        operator<=>(const BasicConstIterator &rhs, const It &lhs) noexcept(noexcept(rhs.m_current <=> lhs.m_current))
            requires ThreeWayComparableWithConcept<Iterator, It>
        {
            return rhs.m_current <=> lhs.m_current;
        }

        template<DifferentFromConcept<BasicConstIterator> It>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr bool
        operator<=>(const It &rhs, const BasicConstIterator &lhs) noexcept(noexcept(rhs.m_current <=> lhs.m_current))
            requires ThreeWayComparableWithConcept<It, Iterator>
        {
            return rhs.m_current <=> lhs.m_current;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr BasicConstIterator
        operator+(const BasicConstIterator &it,
                  DifferenceType diff) noexcept(noexcept(BasicConstIterator(it.m_current + diff)))
            requires RandomAccessIteratorConcept<Iterator>
        {
            return BasicConstIterator(it.m_current + diff);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr BasicConstIterator
        operator+(DifferenceType diff,
                  const BasicConstIterator &it) noexcept(noexcept(BasicConstIterator(diff + it.m_current)))
            requires RandomAccessIteratorConcept<Iterator>
        {
            return BasicConstIterator(diff + it.m_current);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr BasicConstIterator
        operator-(const BasicConstIterator &it,
                  DifferenceType diff) noexcept(noexcept(BasicConstIterator(it.m_current - diff)))
            requires RandomAccessIteratorConcept<Iterator>
        {
            return BasicConstIterator(it.m_current - diff);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr BasicConstIterator
        operator-(DifferenceType diff,
                  const BasicConstIterator &it) noexcept(noexcept(BasicConstIterator(diff - it.m_current)))
            requires RandomAccessIteratorConcept<Iterator>
        {
            return BasicConstIterator(diff - it.m_current);
        }

        template<SizedSentinelForConcept<Iterator> Sentinel, SameAsConcept<Iterator> It>
            requires __impl::__NotBasicConstIterator<Sentinel>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr DifferenceType
        operator-(const Sentinel &sentinel, const BasicConstIterator<It> &it) noexcept(noexcept(sentinel - it.m_current))
        {
            return sentinel - it.m_current;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr RValueReference
        MoveIterator(const BasicConstIterator &it) noexcept(noexcept(static_cast<RValueReference>(MoveIterator(it.m_current))))
        {
            return static_cast<RValueReference>(MoveIterator(it.m_current));
        }

      private:
        Iterator m_current{};
    };

    template<InputIteratorConcept It>
    using ConstIterator = ConditionalType<ConstIteratorConcept<It>, It, BasicConstIterator<It>>;

    template<InputIteratorConcept It>
    constexpr ConstIterator<It> MakeConstIterator(It it) noexcept(IsNoThrowConvertible<It, ConstIterator<It>>)
    {
        return it;
    }

    // NOLINTBEGIN(readability-identifier-naming, bugprone-reserved-identifier)
    namespace __impl
    {
        template<typename Sentinel>
        struct __ConstSentinel
        {
            using Type = Sentinel;
        };

        template<InputIteratorConcept Sentinel>
        struct __ConstSentinel<Sentinel>
        {
            using Type = ConstIterator<Sentinel>;
        };
    } // namespace __impl

    // NOLINTEND(readability-identifier-naming, bugprone-reserved-identifier)

    template<SemiregularConcept Sentinel>
    using ConstSentinel = typename __impl::__ConstSentinel<Sentinel>::Type;

    template<SemiregularConcept Sentinel>
    constexpr ConstSentinel<Sentinel>
    MakeConstSentinel(Sentinel sentinel) noexcept(IsNoThrowConvertible<Sentinel, ConstSentinel<Sentinel>>)
    {
        return sentinel;
    }

} // namespace SSSEngine::Ranges
