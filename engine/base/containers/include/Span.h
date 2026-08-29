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
#include "Array.h"
#include "ArrayTraits.h"
#include "Attributes.h"
#include "BasicIterator.h"
#include "Buffer.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "Iterator.h"
#include "Limits.h"
#include "QualifierTraits.h"
#include "Range.h"
#include "ReverseIterator.h"
#include "Types.h"
#include "Utility.h"
#include "ValueConstant.h"
#include "View.h"

namespace SSSEngine::Containers
{
    using ExtentType = SizeType;

    SSSENGINE_GLOBAL
    constexpr auto DynamicExtent = IntTraits<ExtentType>::Max;

    template<ExtentType Extent>
    struct ExtentStorage
    {
        constexpr ExtentStorage() = default;

        constexpr explicit ExtentStorage(SSSENGINE_MAYBE_UNUSED SizeType extent)
        {
            SSSENGINE_ASSERT(extent == Extent);
        }

        constexpr explicit ExtentStorage(IntegralConstant<Extent>) {}

        SSSENGINE_PURE
        consteval ExtentType GetExtent() const noexcept
        {
            return Extent;
        }
    };

    template<>
    struct ExtentStorage<DynamicExtent>
    {
        SSSENGINE_PURE
        constexpr ExtentType GetExtent() const noexcept
        {
            return extent;
        }

        ExtentType extent{0};
    };

    template<typename T, ExtentType Extent = DynamicExtent>
    class Span
    {
      public:
        using ElementType = T;
        using ValueType = RemoveCVType<T>;
        using PointerType = T *;
        using ConstPointerType = const T *;
        using ReferenceType = ElementType &;
        using ConstReferenceType = const ElementType &;

        using Iterator = Ranges::BasicIterator<PointerType>;
        using ConstIterator = Ranges::BasicIterator<ConstPointerType>;
        using ReverseIterator = Ranges::ReverseIterator<Iterator>;
        using ConstReverseIterator = Ranges::ReverseIterator<ConstIterator>;

      private:
        static constexpr bool IsDynamicSpan = Extent == DynamicExtent;
        static constexpr bool IsEmptySpan = Extent == 0;

        template<typename From>
        static constexpr bool IsValidType = IsArrayConvertible<From, ElementType>;

        template<typename It>
        static constexpr bool IsValidIterator = IsValidType<RemoveReferenceType<Ranges::IteratorReferenceType<It>>>;

        template<typename Range>
        static constexpr bool IsValidRange =
            !IsCStyleArray<Range> && IsValidType<RemoveReferenceType<Ranges::RangeReferenceType<Range>>> &&
            Ranges::SizedRangeConcept<Range> && (Ranges::BorrowedRangeConcept<Range> || IsConst<ElementType>);

        template<typename Array>
        static constexpr bool IsValidArray = IsValidType<RemovePointerType<Ranges::RangeDataType<Array>>>;

        template<ExtentType Offset, ExtentType Count>
        static constexpr ExtentType SubspanExtent = Count != DynamicExtent  ? Count :
                                                    Extent != DynamicExtent ? Extent - Offset :
                                                                              DynamicExtent;
        template<ExtentType Offset, ExtentType Count>
        using SubspanType = Span<ElementType, SubspanExtent<Offset, Count>>;

      public:
        constexpr Span()
            requires(IsDynamicSpan || IsEmptySpan)
        = default;
        constexpr Span(const Span &) noexcept = default;
        constexpr Span(Span &&) = default;
        constexpr Span &operator=(const Span &) = default;
        constexpr Span &operator=(Span &&) = default;
        constexpr ~Span() = default;

        template<Ranges::ContiguousMemoryIteratorConcept It>
            requires(IsValidIterator<It>)
        constexpr explicit(!IsDynamicSpan) Span(It first, ExtentType count) noexcept :
            m_begin(ToAddress(first)), m_count(count)
        {
        }

        template<Ranges::ContiguousMemoryIteratorConcept It, Ranges::SizedSentinelForConcept<It> End>
            requires(IsValidIterator<It>)
        constexpr explicit(!IsDynamicSpan) Span(It first, End end) noexcept(noexcept(end - first)) :
            m_begin(ToAddress(first)), m_count(end - first)
        {
        }

        // NOLINTBEGIN(google-explicit-constructor)

        template<SizeType N>
            requires(IsDynamicSpan || Extent == N)
        constexpr Span(IdentityType<T> (&extent)[N]) noexcept : m_begin(extent), m_count(IntegralConstant<N>{})
        {
        }

        template<Ranges::ContiguousRangeConcept Range>
        constexpr Span(Range &&range) noexcept(noexcept(Ranges::Data(range)) && noexcept(Ranges::Count(range)))
            requires(IsValidRange<decltype(range)>)
            : m_begin(Ranges::Data(range)), m_count(Ranges::Count(range))
        {
        }

        template<typename U, SizeType N>
            requires IsValidArray<Containers::Array<U, N>> && (IsDynamicSpan || Extent == N)
        constexpr Span(Containers::Array<U, N> &array) noexcept : m_begin(array.Data()), m_count(IntegralConstant<N>{})
        {
        }

        template<typename U, SizeType N>
            requires IsValidArray<const Containers::Array<U, N>> && (IsDynamicSpan || Extent == N)
        constexpr Span(const Containers::Array<U, N> &array) noexcept :
            m_begin(array.Data()), m_count(IntegralConstant<N>{})
        {
        }

        // NOLINTEND(google-explicit-constructor)

        template<typename U, SizeType N>
            requires IsValidType<U> && (IsDynamicSpan || N == Extent)
        constexpr explicit(Extent != DynamicExtent && N == DynamicExtent) Span(const Span<U, N> &source) :
            m_begin(source.Data()), m_count(source.Count())
        {
        }

        // Iterators

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator Begin(this Self &&self) noexcept
        {
            return Iterator(Forward<Self>(self).m_begin);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstIterator ConstBegin() const noexcept
        {
            return ConstIterator(m_begin);
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReverseIterator ReverseBegin(this Self &&self) noexcept
        {
            return ReverseIterator(Forward<Self>(self).End());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstReverseIterator ConstReverseBegin() const noexcept
        {
            return ConstReverseIterator(ConstEnd());
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator End(this Self &&self) noexcept
        {
            return Iterator(Forward<Self>(self).m_begin + Forward<Self>(self).Count());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstIterator ConstEnd() const noexcept
        {
            return ConstIterator(m_begin + Count());
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReverseIterator ReverseEnd(this Self &&self) noexcept
        {
            return ReverseIterator(Forward<Self>(self).Begin());
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstReverseIterator ConstReverseEnd() const noexcept
        {
            return ConstReverseIterator(Begin());
        }

        // Element Access

        template<typename Self>
            requires(!IsEmptySpan)
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReferenceType Front(this Self &&self) noexcept
        {
            SSSENGINE_ASSERT(!Forward<Self>().IsEmpty());
            return *Forward<Self>(self).m_begin;
        }

        template<typename Self>
            requires(!IsEmptySpan)
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReferenceType Back(this Self &&self) noexcept
        {
            SSSENGINE_ASSERT(!Forward<Self>().IsEmpty());
            return *(Forward<Self>(self).m_begin + (Forward<Self>(self).Count() - 1));
        }

        template<typename Self>
            requires(!IsEmptySpan)
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReferenceType operator[](this Self &&self, ExtentType index) noexcept
        {
            SSSENGINE_ASSERT(!Forward<Self>().IsEmpty());
            SSSENGINE_ASSERT(Forward<Self>().Count() > index);
            return Forward<Self>(self).m_begin[index];
        }

        template<typename Self>
            requires(!IsEmptySpan)
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Optional<ReferenceType> TryAt(this Self &&self, ExtentType index) noexcept
        {
            if(Forward<Self>(self).IsEmpty || Forward<Self>(self).Count() <= index)
                return Optional<ReferenceType>{};

            return Optional<ReferenceType>(Forward<Self>(self).m_begin[index]);
        }

        template<typename Self>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr PointerType Data(this Self &&self) noexcept
        {
            return Forward<Self>(self).m_begin;
        }

        // Observers

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ExtentType Count() const noexcept
        {
            return m_count.GetExtent();
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr bool IsEmpty() const noexcept
        {
            return Count() == 0;
        }

        // Subspans

        template<ExtentType Offset, ExtentType Size = DynamicExtent>
            requires(Offset <= Extent) && (Size == DynamicExtent || Size <= Extent - Offset)
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr SubspanType<Offset, Size> Subspan() const noexcept
        {
            if constexpr(IsDynamicSpan)
            {
                SSSENGINE_ASSERT(Offset <= Count());
                SSSENGINE_ASSERT(Size == DynamicExtent || Size <= Count() - Offset);
            }

            constexpr auto Start = m_begin + Offset;

            if constexpr(Size == DynamicExtent)
            {
                return SubspanType<Offset, Size>(Start, Count() - Offset);
            }
            else
            {
                return SubspanType<Offset, Size>(Start, Size);
            }
        }

        SSSENGINE_PURE
        constexpr Span<ElementType, DynamicExtent> Subspan(ExtentType offset, ExtentType count = DynamicExtent) const noexcept
        {
            SSSENGINE_ASSERT(offset <= Count());

            constexpr auto Start = m_begin + Offset;

            if(count == DynamicExtent)
            {
                return Span<ElementType, DynamicExtent>(Start, Count() - offset);
            }

            SSSENGINE_ASSERT(count <= Count() - offset);
            return Span<ElementType, DynamicExtent>(Start, count);
        }

        template<ExtentType Size>
            requires(Size <= Extent)
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Span<ElementType, Size> FirstN() const noexcept
        {
            if constexpr(IsDynamicSpan)
            {
                SSSENGINE_ASSERT(Count() >= Size);
            }
            return Span<ElementType, Size>(m_begin, Size);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Span<ElementType, DynamicExtent> FirstN(ExtentType count) const noexcept
        {
            SSSENGINE_ASSERT(Count() >= count);
            return Span<ElementType, DynamicExtent>(m_begin, count);
        }

        template<ExtentType Size>
            requires(Size <= Extent)
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Span<ElementType, Size> LastN() const noexcept
        {
            if constexpr(IsDynamicSpan)
            {
                SSSENGINE_ASSERT(Count() >= Size);
            }
            return Span<ElementType, Size>(m_begin + (Count() - Size), Size);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Span<ElementType, DynamicExtent> LastN(ExtentType count) const noexcept
        {
            SSSENGINE_ASSERT(Count() >= count);
            return Span<ElementType, DynamicExtent>(m_begin + (Count() - count), count);
        }

      private:
        PointerType m_begin = nullptr;
        SSSENGINE_OVERLAP ExtentStorage<Extent> m_count;

        // NOLINTBEGIN(readability-identifier-naming)

        friend constexpr Iterator begin(const Span &span)
        {
            return span.Begin();
        }

        friend constexpr Iterator end(const Span &span)
        {
            return span.End();
        }

        // NOLINTEND(readability-identifier-naming)
    }; // namespace SSSEngine::Containers

    // Deduction Guides

    template<typename T>
    concept IntegralConstantLikeConcept = IsInteger<RemoveCVReferenceType<decltype(T::Value)>>;

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr ExtentType MaybeDynamicExtent = false;

    template<IntegralConstantLikeConcept T>
    SSSENGINE_GLOBAL
    constexpr ExtentType MaybeDynamicExtent<T> = T::Value;

    template<typename It, typename EndOrSize>
    Span(It, EndOrSize) -> Span<RemoveReferenceType<Ranges::IteratorReferenceType<It>>, MaybeDynamicExtent<EndOrSize>>;

    template<typename T, SizeType N>
    Span(T (&)[N]) -> Span<T, N>;

    template<typename T, SizeType N>
    Span(Containers::Array<T, N> &) -> Span<T, N>;

    template<typename T, SizeType N>
    Span(const Containers::Array<T, N> &) -> Span<const T, N>;

    template<Ranges::RangeConcept Range>
    Span(Range &&) -> Span<RemoveReferenceType<Ranges::RangeReferenceType<Range>>>;

} // namespace SSSEngine::Containers

namespace SSSEngine::Ranges
{
    template<typename T, Containers::ExtentType N>
    SSSENGINE_GLOBAL
    constexpr bool EnableBorrowRange<Containers::Span<T, N>> = true;

    template<typename T, Containers::ExtentType N>
    SSSENGINE_GLOBAL
    constexpr bool EnableView<Containers::Span<T, N>> = true;

} // namespace SSSEngine::Ranges
