/**
 * @file
 * @brief
 */

#pragma once

#include "Attributes.h"
#include "QualifierTraits.h"
#include "Iterator.h"
#include "Ordering.h"

namespace SSSEngine::Ranges
{

    /**
     * @class BasicIterator
     * @brief Wrapper for pointer types for iterating
     *
     * @tparam Iterator The type of iterator
     */
    template<typename Iterator>
        requires(IsPointer<Iterator>)
    class BasicIterator
    {
        using IteratorTraits = IteratorTraits<Iterator>;

      public:
        using ValueType = IteratorTraits::ValueType;
        using PointerType = IteratorTraits::PointerType;
        using ReferenceType = IteratorTraits::ReferenceType;
        using DifferenceType = IteratorTraits::DifferenceType;
        using ConstIterator = BasicIterator<const ValueType *>;

        explicit BasicIterator(const Iterator &it) : m_it(it) {}

        BasicIterator(const BasicIterator &it) = default;
        BasicIterator(BasicIterator &&) = default;
        BasicIterator &operator=(const BasicIterator &) = default;
        BasicIterator &operator=(BasicIterator &&) = default;
        ~BasicIterator() = default;

        // NOLINTBEGIN(google-explicit-constructor)

        operator ConstIterator()
        {
            return ConstIterator{m_it};
        }

        // NOLINTEND(google-explicit-constructor)

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ReferenceType operator*() const noexcept
        {
            return *m_it;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr PointerType operator->() const noexcept
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
        friend constexpr DifferenceType operator+(BasicIterator lhs, BasicIterator rhs) noexcept
        {
            return lhs.m_it + rhs.m_it;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        friend constexpr DifferenceType operator-(BasicIterator lhs, BasicIterator rhs) noexcept
        {
            return lhs.m_it - rhs.m_it;
        }

        Iterator m_it;
    };

    template<typename T>
    struct IteratorTraits<BasicIterator<T>> : public IteratorTraits<T>
    {
    };

} // namespace SSSEngine::Ranges
