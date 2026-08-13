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
#include "Concepts.h"
#include "HelperMacros.h"
#include "ObjectConcepts.h"
#include "QualifierTraits.h"
#include "SignTraits.h"
#include "Traits.h"
#include "Types.h"
#include "Utility.h"
#include "Address.h"

namespace SSSEngine::Ranges
{
    template<typename T>
    concept WeaklyIncrementableConcept = MovableConcept<T> && requires(T i) {
        { ++i } -> SameAsConcept<T &>;
        { i++ };
    };

    template<typename T>
    concept IncrementableConcept = RegularConcept<T> && WeaklyIncrementableConcept<T> && requires(T i) {
        { i++ } -> SameAsConcept<T>;
    };

    template<typename T>
    concept WeaklyDecrementableConcept = MovableConcept<T> && requires(T i) {
        { --i } -> SameAsConcept<T &>;
        { i-- };
    };

    template<typename T>
    concept DecrementableConcept = RegularConcept<T> && WeaklyDecrementableConcept<T> && requires(T i) {
        { i-- } -> SameAsConcept<T>;
    };

    template<typename It>
    struct IteratorTraits
    {
    };

    template<typename T>
    struct IteratorTraits<T *>
    {
        using ValueType = T;
        using DifferenceType = ptrdiff;
        using PointerType = T *;
        using ReferenceType = T &;
    };

    template<typename T>
    struct IteratorTraits<const T *>
    {
        using ValueType = T;
        using DifferenceType = ptrdiff;
        using PointerType = const T *;
        using ReferenceType = const T &;
    };

    namespace Impl
    {
        struct DecayCopy
        {
            template<typename T>
            constexpr DecayType<T> operator()(T &&t) const noexcept(IsNoThrowConvertible<T, DecayType<T>>)
            {
                return Forward<T>(t);
            }
        } inline constexpr DecayCopy{};

        template<typename T>
        struct IteratorValue
        {
        };

        template<ObjectConcept T>
        struct IteratorValue<T *>
        {
            using ValueType = RemoveCVType<T>;
        };

        template<ArrayConcept T>
        struct IteratorValue<T>
        {
            using ValueType = RemoveCVType<RemoveExtentType<T>>;
        };

        template<HasValueTypeConcept T>
        struct IteratorValue<T>
        {
            using ValueType = typename T::ValueType;
        };

        template<HasElementTypeConcept T>
        struct IteratorValue<T>
        {
            using ValueType = typename T::ElementType;
        };

        template<typename>
        struct IteratorDifference
        {
        };

        template<ObjectConcept T>
        struct IteratorDifference<T *>
        {
            using DifferenceType = ptrdiff;
        };

        template<typename T>
        struct IteratorDifference<const T> : IteratorDifference<T>
        {
        };

        template<typename T>
            requires requires { typename T::DifferenceType; }
        struct IteratorDifference<T>
        {
            using DifferenceType = T::DifferenceType;
        };

        template<typename T>
            requires(!requires { typename T::DifferenceType; }) && requires(T a, T b) {
                { a - b } -> IntegralConcept;
            }
        struct IteratorDifference<T>
        {
            using DifferenceType = SignedType<decltype(DeclVal<T>() - DeclVal<T>())>;
        };

        void IteratorMove() = delete;

        template<typename T>
        concept HasIteratorMoveConcept =
            ClassOrEnumConcept<RemoveReferenceType<T>> && requires(T &&t) { IteratorMove(static_cast<T &&>(t)); };

        class IteratorMoveImpl
        {
          private:
            template<typename T>
            using RefType = decltype(*DeclVal<T>());

            template<typename T>
            struct Result
            {
                using Type = RefType<T>;
            };

            template<HasIteratorMoveConcept T>
            struct Result<T>
            {
                using Type = decltype(IteratorMove(DeclVal<T>()));
            };

            template<typename T>
                requires(!HasIteratorMoveConcept<T>) && IsLValueReference<RefType<T>>
            struct Result<T>
            {
                using Type = decltype(DeclVal<RemoveReferenceType<RefType<T>>>());
            };

            template<typename T>
            static constexpr bool IsNoExcept()
            {
                if constexpr(HasIteratorMoveConcept<T>)
                {
                    return noexcept(IteratorMove(DeclVal<T>()));
                }
                else
                {
                    return noexcept(*DeclVal<T>());
                }
            }

          public:
            template<typename T>
            using Type = typename Result<T>::Type;

            template<typename T>
                requires HasIteratorMoveConcept<T> || requires { typename RefType<T>; }
            SSSENGINE_PURE
            constexpr Type<T> operator()(T &&t) const noexcept(IsNoExcept<T>())
            {
                if constexpr(HasIteratorMoveConcept<T>)
                {
                    return IteratorMove(static_cast<T &&>(t));
                }
                else if constexpr(IsLValueReference<RefType<T>>)
                {
                    return Move(*static_cast<T &&>(t));
                }
                else
                {
                    return *static_cast<T &&>(t);
                }
            }
        };
    } // namespace Impl

    SSSENGINE_GLOBAL
    constexpr Impl::IteratorMoveImpl IteratorMove{};

    template<typename T>
        requires requires { typename Impl::IteratorValue<T>::ValueType; }
    using IteratorValueType = Impl::IteratorValue<T>::ValueType;

    template<DereferenceableConcept T>
    using IteratorReferenceType = decltype(*DeclVal<T &>());

    template<DereferenceableConcept T>
    using IteratorRValueReferenceType = decltype(IteratorMove(DeclVal<T &>()));

    template<DereferenceableConcept T>
    using IteratorDifferenceType = Impl::IteratorDifference<T>::DifferenceType;

    namespace Impl
    {
        template<typename T>
        concept IndirectlyReadableConcept =
            requires {
                typename IteratorValueType<T>;
                typename IteratorReferenceType<T>;
                typename IteratorRValueReferenceType<T>;
                requires SameAsConcept<IteratorReferenceType<const T>, IteratorReferenceType<T>>;
                requires SameAsConcept<IteratorRValueReferenceType<const T>, IteratorRValueReferenceType<T>>;
            } && CommonReferenceWithConcept<IteratorReferenceType<T> &&, IteratorValueType<T> &> &&
            CommonReferenceWithConcept<IteratorReferenceType<T> &&, IteratorRValueReferenceType<T> &&> &&
            CommonReferenceWithConcept<IteratorRValueReferenceType<T> &&, const IteratorValueType<T> &>;
    } // namespace Impl

    template<typename T>
    concept IndirectlyReadableConcept = Impl::IndirectlyReadableConcept<RemoveCVReferenceType<T>>;

    template<typename Out, typename T>
    concept IndirectlyWritableConcept = requires(Out &&o, T &&t) {
        *o = Forward<T>(t);
        *Forward<Out>(o) = Forward<T>(t);
        const_cast<const IteratorReferenceType<Out> &&>(*o) = Forward<T>(t);
        const_cast<const IteratorReferenceType<Out> &&>(*Forward<Out>(o)) = Forward<T>(t);
    };

    template<typename Iterator>
    concept IteratorConcept = requires(Iterator i) {
        { *i } -> ReferenceableConcept;
    } && WeaklyIncrementableConcept<Iterator>;

    template<typename Iterator>
    concept ReadIteratorConcept = IteratorConcept<Iterator> && IndirectlyReadableConcept<Iterator>;

    template<typename Iterator>
    concept InputIteratorConcept = ReadIteratorConcept<Iterator>;

    template<typename Iterator, typename T>
    concept WriteIteratorConcept = IteratorConcept<Iterator> && IndirectlyWritableConcept<Iterator, T> &&
                                   requires(Iterator &i, T &&t) { *i++ = Forward<T>(t); };

    template<typename Iterator, typename T>
    concept OutputIteratorConcept = WriteIteratorConcept<Iterator, T>;

    template<typename Sentinel, typename Iterator>
    concept SentinelForConcept =
        SemiregularConcept<Sentinel> && IteratorConcept<Iterator> && EqualityComparableWithConcept<Sentinel, Iterator>;

    template<typename Sentinel, typename Iterator>
    SSSENGINE_GLOBAL
    constexpr bool DisableDistanceSentinelFor = false;

    template<typename Sentinel, typename Iterator>
    concept SizedSentinelForConcept =
        SentinelForConcept<Sentinel, Iterator> && !DisableDistanceSentinelFor<RemoveCV<Sentinel>, RemoveCV<Iterator>> &&
        requires(const Sentinel &s, const Iterator &i) {
            { s - i } -> SameAsConcept<IteratorDifferenceType<Iterator>>;
            { i - s } -> SameAsConcept<IteratorDifferenceType<Iterator>>;
        };

    template<typename Iterator>
    concept MultiPassIteratorConcept =
        InputIteratorConcept<Iterator> && IncrementableConcept<Iterator> && SentinelForConcept<Iterator, Iterator>;

    template<typename Iterator, typename T>
    concept WriteableMultiPassIteratorConcept = MultiPassIteratorConcept<Iterator> && WriteIteratorConcept<Iterator, T>;

    template<typename Iterator>
    concept BidirectionalIteratorConcept = MultiPassIteratorConcept<Iterator> && DecrementableConcept<Iterator>;

    template<typename Iterator, typename T>
    concept WriteableBidirectionalIteratorConcept =
        BidirectionalIteratorConcept<Iterator> && WriteIteratorConcept<Iterator, T>;

    template<typename Iterator>
    concept RandomAccessIteratorConcept =
        BidirectionalIteratorConcept<Iterator> && TotallyComparableConcept<Iterator> &&
        SizedSentinelForConcept<Iterator, Iterator> &&
        requires(Iterator i, const Iterator j, const IteratorDifferenceType<Iterator> n) {
            { i += n } -> SameAsConcept<Iterator &>;
            { i -= n } -> SameAsConcept<Iterator &>;
            { j + n } -> SameAsConcept<Iterator>;
            { j - n } -> SameAsConcept<Iterator>;
            { n + j } -> SameAsConcept<Iterator>;
            { j[n] } -> SameAsConcept<IteratorReferenceType<Iterator>>;
        };

    template<typename Iterator, typename T>
    concept WriteableRandomAccessIteratorConcept =
        RandomAccessIteratorConcept<Iterator> && WriteIteratorConcept<Iterator, T>;

    template<typename Iterator>
    concept ContiguousMemoryIteratorConcept =
        RandomAccessIteratorConcept<Iterator> && IsLValueReference<IteratorReferenceType<Iterator>> &&
        SameAsConcept<IteratorValueType<Iterator>, RemoveCVReferenceType<IteratorReferenceType<Iterator>>> &&
        requires(const Iterator &i) {
            { ToAddress(i) } -> SameAsConcept<AddPointerType<IteratorReferenceType<Iterator>>>;
        };

    template<typename Iterator, typename T>
    concept WriteableContiguousMemoryIteratorConcept =
        ContiguousMemoryIteratorConcept<Iterator> && WriteIteratorConcept<Iterator, T>;

} // namespace SSSEngine::Ranges
