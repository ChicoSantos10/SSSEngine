/**
 * @file
 * @brief
 */

#pragma once

#include "Attributes.h"
#include "Concepts.h"
#include "ConstIterator.h"
#include "ConversionTraits.h"
#include "Debug.h"
#include "QualifierTraits.h"
#include "Iterator.h"
#include "Ordering.h"

namespace SSSEngine::Ranges
{

    /**
     * @class BasicIterator
     * @brief Adaptor for iterators that are pure pointer iterators
     *
     * @tparam Iterator The type of iterator
     */
    template<PointerConcept Iterator>
    class BasicIterator
    {
      public:
        using ValueType = IteratorValueType<Iterator>;
        using ReferenceType = IteratorReferenceType<Iterator>;
        using DifferenceType = IteratorDifferenceType<Iterator>;

        explicit BasicIterator(const Iterator &it) : m_it(it) {}

        BasicIterator() = default;
        BasicIterator(const BasicIterator &it) = default;
        BasicIterator(BasicIterator &&) = default;
        BasicIterator &operator=(const BasicIterator &) = default;
        BasicIterator &operator=(BasicIterator &&) = default;
        ~BasicIterator() = default;

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
        constexpr BasicIterator operator+(DifferenceType offset) const noexcept
        {
            return BasicIterator(m_it + offset);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr BasicIterator &operator-(DifferenceType offset) const noexcept
        {
            return BasicIterator(m_it - offset);
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr Iterator Current() const noexcept
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

        Iterator m_it{};
    };

} // namespace SSSEngine::Ranges
