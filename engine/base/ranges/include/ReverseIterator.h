/**
 * @file
 * @brief
 */

#pragma once

#include "Address.h"
#include "Attributes.h"
#include "ConstIterator.h"
#include "Iterator.h"
#include "Ordering.h"

namespace SSSEngine::Ranges
{
    /**
     * @class ReverseIterator
     * @brief A wrapper for an Iterator that advances from the end to the beginning of a sequence.
     *
     * @tparam Iterator The type of bidirectional iterator
     */
    template<BidirectionalIteratorConcept Iterator>
    class ReverseIterator
    {
      public:
        using ValueType = IteratorValueType<Iterator>;
        using ReferenceType = IteratorReferenceType<Iterator>;
        using DifferenceType = IteratorDifferenceType<Iterator>;
        using ConstIterator = BasicConstIterator<Iterator>;

        constexpr explicit ReverseIterator(const Iterator &it) : m_it(it) {}

        constexpr ReverseIterator(const ReverseIterator &it) = default;
        constexpr ReverseIterator(ReverseIterator &&) = default;
        constexpr ReverseIterator &operator=(const ReverseIterator &) = default;
        constexpr ReverseIterator &operator=(ReverseIterator &&) = default;
        constexpr ~ReverseIterator() = default;

        // NOLINTBEGIN(google-explicit-constructor)

        operator ReverseIterator<ConstIterator>() const noexcept
        {
            return ReverseIterator<ConstIterator>(ConstIterator(m_it));
        }

        // NOLINTEND(google-explicit-constructor)

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReferenceType operator*() const noexcept
        {
            auto temp = m_it;
            return *--temp;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto *operator->() const noexcept
        {
            auto temp = m_it;
            --temp;

            return ToAddress(temp);
        }

        SSSENGINE_FORCE_INLINE
        constexpr ReverseIterator &operator++() noexcept
        {
            --m_it;
            return *this;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReverseIterator operator++(int) noexcept
        {
            return ReverseIterator(m_it--);
        }

        SSSENGINE_FORCE_INLINE
        constexpr ReverseIterator &operator--() noexcept
        {
            ++m_it;
            return *this;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReverseIterator operator--(int) noexcept
        {
            return ReverseIterator(m_it++);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReferenceType operator[](DifferenceType index) const noexcept
        {
            return *(*this + index);
        }

        SSSENGINE_FORCE_INLINE
        constexpr ReverseIterator &operator+=(DifferenceType offset) noexcept
        {
            m_it -= offset;
            return *this;
        }

        SSSENGINE_FORCE_INLINE
        constexpr ReverseIterator &operator-=(DifferenceType offset) noexcept
        {
            m_it += offset;
            return *this;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReverseIterator operator+(DifferenceType offset) const noexcept
        {
            return ReverseIterator(m_it - offset);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReverseIterator &operator-(DifferenceType offset) const noexcept
        {
            return ReverseIterator(m_it + offset);
        }

      private:
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr bool operator==(ReverseIterator lhs, ReverseIterator rhs) noexcept
        {
            return lhs.m_it == rhs.m_it;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr auto operator<=>(ReverseIterator lhs, ReverseIterator rhs) noexcept
        {
            return lhs.m_it <=> rhs.m_it;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr DifferenceType operator+(ReverseIterator lhs, ReverseIterator rhs) noexcept
        {
            return lhs.m_it + rhs.m_it;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr DifferenceType operator-(ReverseIterator lhs, ReverseIterator rhs) noexcept
        {
            return lhs.m_it - rhs.m_it;
        }

        Iterator m_it;
    };

    template<BidirectionalIteratorConcept It>
    constexpr ReverseIterator<It> MakeReverseIterator(It iterator)
    {
        return ReverseIterator<It>(iterator);
    }

} // namespace SSSEngine::Ranges
