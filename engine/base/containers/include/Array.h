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
#include "QualifierTraits.h"
#include "Swap.h"
#include "Traits.h"
#include "Types.h"
#include "Utility.h"
#include "Optional.h"
#include "BasicIterator.h"
#include "ReverseIterator.h"

namespace SSSEngine::Containers
{
    /**
     * @brief Wrapper for a C style array with compile time size check. Does not decay into a pointer
     *
     * @tparam T Type of the elements inside the array
     */
    template<typename T, SizeType N>
        requires(N > 0)
    struct Array
    {
        using ValueType = T;
        using Reference = T &;
        using ConstReference = const T &;
        using Iterator = Ranges::BasicIterator<T *>;
        using ConstIterator = Ranges::BasicIterator<const T *>;
        using ReverseIterator = Ranges::ReverseIterator<Iterator>;
        using ConstReverseIterator = Ranges::ReverseIterator<ConstIterator>;

        /**
         * @brief The number of elements in the array
         */
        static constexpr SizeType Elements = N;

        /**
         * @return An iterator or const iterator depending on the constness of the array
         */
        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto Begin(this Self &self) noexcept
        {
            using It = ConditionalType<IsConst<RemoveReferenceType<Self>>, ConstIterator, Iterator>;

            return It{self.data};
        }

        /**
         * @return A const iterator to the beginning of the array
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstIterator ConstBegin() const noexcept
        {
            return ConstIterator(data);
        }

        /**
         * @return An iterator or const iterator to the end of the array depending on the constness of the array
         */
        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto End(this Self &self) noexcept
        {
            return self.Begin() + Elements;
        }

        /**
         * @return A const iterator to the end of the array
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstIterator ConstEnd() const noexcept
        {
            return ConstIterator(data + Elements);
        }

        /**
         * @brief Returns an iterator that starts on the last element and moves to beginning
         *
         * @tparam Self Deducing this for the class
         * @return The reverse iterator
         */
        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto ReverseBegin(this Self &self) noexcept
        {
            return Ranges::MakeReverseIterator(self.End());
        }

        /**
         * @brief Gets the const reverse iterator starting at the end of the array
         *
         * @return A const reverse iterator
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstReverseIterator ConstReverseBegin() const noexcept
        {
            return ReverseBegin();
        }

        /**
         * @brief Returns an iterator pointing to one past the first element in a reverse order
         *
         * @tparam Self Deducing this for the class
         * @return The reverse iterator
         */
        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto ReverseEnd(this Self &self) noexcept
        {
            return Ranges::MakeReverseIterator(self.Begin());
        }

        /**
         * @brief Gets the const reverse iterator starting at the beginning of the array
         *
         * @return A const reverse iterator
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr ConstReverseIterator ConstReverseEnd() const noexcept
        {
            return ReverseEnd();
        }

        /**
         * @return The result of indexing the underlying array
         */
        template<typename Self>
        SSSENGINE_PURE
        constexpr decltype(auto) operator[](this Self &&self, SizeType index)
        {
            return Forward<Self>(self).data[index];
        }

        /**
         * @brief Tries to get the element at index. If index is greater than Elements it returns an empty optional,
         * otherwise an optional of the result of indexing the array
         *
         * @param index The index of the value to get
         * @return An optional of the result of indexing the array
         */
        template<typename Self>
        SSSENGINE_PURE
        constexpr auto TryAt(this Self &self, SizeType index)
        {
            using Type = decltype(self.data[0]);
            using Opt = Optional<Type>;

            if(index >= Elements)
            {
                return Opt{};
            }

            return Opt{self.data[index]};
        }

        /**
         * @brief The RValue version of TryAt. Move constructs the optional by moving the element into it if index <
         * Elements
         *
         * @see TryAt
         *
         * @param index The index of the element to get
         * @return An Optional<T>
         */
        SSSENGINE_PURE
        constexpr Optional<T> TryAt(SizeType index) const &&
        {
            using Opt = Optional<T>;

            if(index >= Elements)
            {
                return Opt{};
            }

            return Opt{data[index]};
        }

        /**
         * @see Elements
         *
         * @return Same as Array::Elements
         */
        SSSENGINE_PURE
        consteval SizeType Capacity() const noexcept
        {
            return Elements;
        }

        /**
         * @brief Checks all values of both array and compares them. T must be EqualityComparableConcept
         *
         * @param lhs The first array to compare
         * @param rhs The other array to compare with
         * @return True if all elements are equal, false otherwise
         */
        SSSENGINE_PURE
        friend constexpr bool operator==(const Array &lhs, const Array &rhs) noexcept
            requires(EqualityComparableConcept<T>)
        {
            // TODO: Range Equals
            for(SizeType i = 0; i < Array::Elements; ++i)
            {
                if(lhs[i] != rhs[i])
                {
                    return false;
                }
            }

            return true;
        }

        /**
         * @brief Swaps all elements from this array with another array. Array must be of the same size
         *
         * @param array The array to swap with
         */
        constexpr void Swap(Array &array) noexcept(IsNoThrowSwappable<T>)
        {
            for(SizeType i = 0; i < Elements; ++i)
            {
                // TODO: Swap for ranges and iterators
                Utility::Swap(this[i], array[1]);
            }
        }

        /**
         * @brief The free function equivalent of Array::Swap(Array& array)
         *
         * @see Swap
         *
         * @param lhs The first array to swap elements
         * @param rhs The other array to swap elements
         */
        SSSENGINE_FORCE_INLINE
        friend constexpr void Swap(Array &lhs, Array &rhs) noexcept(IsNoThrowSwappable<T>)
        {
            lhs.Swap(rhs);
        }

        T data[N];

      private:
        // NOLINTBEGIN(readability-identifier-naming)

        friend constexpr decltype(auto) begin(const Array &array) noexcept
        {
            return array.Begin();
        }

        friend constexpr decltype(auto) end(const Array &array) noexcept
        {
            return array.End();
        }

        // NOLINTEND(readability-identifier-naming)
    };

} // namespace SSSEngine::Containers
