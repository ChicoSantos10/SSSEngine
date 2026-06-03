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
#include "ArrayTraits.h"
#include "Attributes.h"
#include "Concepts.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "Iterator.h"
#include "QualifierTraits.h"
#include "SignTraits.h"
#include "Traits.h"

namespace SSSEngine::Iterators
{
    namespace Impl
    {
        template<typename T>
        concept AdlBeginConcept = ClassOrEnumConcept<RemoveReferenceType<T>> && requires(T &t) {
            { DecayCopy(begin(t)) } -> IteratorConcept;
        };

        template<typename T>
        concept HasBeginConcept = requires(T &t) {
            { t.Begin() } -> IteratorConcept;
        };

        class BeginImpl
        {
          private:
            template<typename T>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            static constexpr bool IsNoExcept()
            {
                if constexpr(IsArray<RemoveReferenceType<T>>)
                {
                    return true;
                }
                else if constexpr(HasBeginConcept<T>)
                {
                    return noexcept(DecayCopy(DeclVal<T &>().Begin()));
                }
                else
                {
                    return noexcept(DecayCopy(begin(DeclVal<T &>())));
                }
            }

          public:
            template<typename T>
                requires IsArray<RemoveReferenceType<T>> || HasBeginConcept<T> || AdlBeginConcept<T>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&t) const noexcept(IsNoExcept<T &>())
            {
                if constexpr(IsArray<RemoveReferenceType<T>>)
                {
                    SSSENGINE_STATIC_ASSERT(IsLValueReference<T>);
                    return t + 0;
                }
                else if constexpr(HasBeginConcept<T>)
                {
                    return t.Begin();
                }
                else
                {
                    return begin(t);
                }
            }
        };

        template<typename T>
        using RangeIteratorType = decltype(BeginImpl{}(DeclVal<T &>()));

        template<typename T>
        concept AdlEndConcept = ClassOrEnumConcept<RemoveReferenceType<T>> && requires(T &t) {
            { DecayCopy(end(t)) } -> SentinelForConcept<RangeIteratorType<T>>;
        };

        template<typename T>
        concept HasEndConcept = requires(T &t) {
            { t.End() } -> SentinelForConcept<RangeIteratorType<T>>;
        };

        class EndImpl
        {
          private:
            template<typename T>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            static constexpr bool IsNoExcept()
            {
                if constexpr(IsArrayKnownBounds<RemoveReferenceType<T>>)
                {
                    return true;
                }
                else if constexpr(HasEndConcept<T>)
                {
                    return noexcept(DecayCopy(DeclVal<T &>().End()));
                }
                else
                {
                    return noexcept(DecayCopy(end(DeclVal<T &>())));
                }
            }

          public:
            template<typename T>
                requires IsArray<RemoveReferenceType<T>> || HasEndConcept<T> || AdlEndConcept<T>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&t) const noexcept(IsNoExcept<T &>())
            {
                if constexpr(IsArrayKnownBounds<RemoveReferenceType<T>>)
                {
                    SSSENGINE_STATIC_ASSERT(IsLValueReference<T>);
                    return t + CountOf<T>;
                }
                else if constexpr(HasEndConcept<T>)
                {
                    return t.End();
                }
                else
                {
                    return end(t);
                }
            }
        };

        template<typename T>
        concept AdlCountConcept = ClassOrEnumConcept<RemoveReferenceType<T>> && requires(T &t) {
            { DecayCopy(Count(t)) } -> IntegralConcept;
        };

        template<typename T>
        concept HasCountConcept = requires(T &t) {
            { t.Count() } -> IntegralConcept;
        };

        template<typename T>
        auto ToUnsigned(T t)
        {
            return static_cast<UnsignedType<T>>(t);
        }

        template<typename T>
        concept SentinelCountConcept = requires(T &t) {
            requires(!IsArrayUnknownBounds<RemoveReferenceType<T>>);
            { BeginImpl{}(t) } -> IteratorConcept;
            { EndImpl{}(t) } -> SizedSentinelForConcept<decltype(BeginImpl{}(t))>;
            ToUnsigned(EndImpl{}(t)-BeginImpl{}(t));
        };

        class CountImpl
        {
          private:
            template<typename T>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            static constexpr bool IsNoExcept()
            {
                if constexpr(IsArrayKnownBounds<RemoveReferenceType<T>>)
                {
                    return true;
                }
                else if constexpr(HasCountConcept<T>)
                {
                    return noexcept(DecayCopy(DeclVal<T &>().Count()));
                }
                else if constexpr(AdlCountConcept<T>)
                {
                    return noexcept(DecayCopy(Count(DeclVal<T &>())));
                }
                else if constexpr(SentinelCountConcept<T>)
                {
                    return noexcept(EndImpl{}(DeclVal<T &>()) - BeginImpl{}(DeclVal<T &>()));
                }
            }

          public:
            template<typename T>
                requires IsArrayKnownBounds<RemoveReferenceType<T>> || HasCountConcept<T> || AdlCountConcept<T> ||
                         SentinelCountConcept<T>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&t) const noexcept(IsNoExcept<T &>())
            {
                if constexpr(IsArrayKnownBounds<RemoveReferenceType<T>>)
                {
                    return CountOf<RemoveReferenceType<T>>;
                }
                else if constexpr(HasCountConcept<T>)
                {
                    return t.Count();
                }
                else if constexpr(AdlCountConcept<T>)
                {
                    return Count(t);
                }
                else if constexpr(SentinelCountConcept<T>)
                {
                    return ToUnsigned(EndImpl{}(t)-BeginImpl{}(t));
                }
            }
        };

        template<typename T>
        using DataResult = AddPointerType<RemoveReferenceType<T>>;

        template<typename T>
        concept AdlDataConcept = ClassOrEnumConcept<RemoveReferenceType<T>> && requires(T &t) {
            { DecayCopy(Data(t)) } -> SameAsConcept<DataResult<T>>;
        };

        template<typename T>
        concept HasDataConcept = requires(T &t) {
            { t.Data() } -> SameAsConcept<DataResult<T>>;
        };

        template<typename T>
        concept DataFromBeginConcept = ContiguousMemoryIteratorConcept<RangeIteratorType<T>>;

        class DataImpl
        {
          private:
            template<typename T>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            static constexpr bool IsNoExcept()
            {
                if constexpr(HasDataConcept<T>)
                {
                    return noexcept(DecayCopy(DeclVal<T &>().Data()));
                }
                else if(AdlDataConcept<T>)
                {
                    return noexcept(DecayCopy(Data(DeclVal<T &>())));
                }
                else
                {
                    return noexcept(BeginImpl{}(DeclVal<T &>()));
                }
            }

          public:
            template<typename T>
                requires HasDataConcept<T> || AdlDataConcept<T> || DataFromBeginConcept<T>
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&t) const noexcept(IsNoExcept<T &>())
            {
                if constexpr(HasDataConcept<T>)
                {
                    return t.Data();
                }
                else if(AdlDataConcept<T>)
                {
                    return data(t);
                }
                else
                {
                    return ToAddress(BeginImpl{}(t));
                }
            }
        };
    } // namespace Impl

    inline namespace Utility
    {
        SSSENGINE_GLOBAL
        constexpr Impl::BeginImpl Begin{};

        SSSENGINE_GLOBAL
        constexpr Impl::EndImpl End{};

        SSSENGINE_GLOBAL
        constexpr Impl::CountImpl Count{};

        SSSENGINE_GLOBAL
        constexpr Impl::DataImpl Data{};
    } // namespace Utility

    template<typename R>
    concept RangeConcept = requires(R &r) {
        Iterators::Begin(r);
        Iterators::End(r);
    };

    template<typename R>
    using IteratorType = Impl::RangeIteratorType<R>;

    template<typename R>
    using SentinelType = decltype(End(DeclVal<R &>()));

    template<typename R>
    using RangeValueType = IteratorValueType<IteratorType<R>>;

    template<typename R>
    using RangeReferenceType = IteratorReferenceType<IteratorType<R>>;

    template<typename R>
    using RangeRValueReferenceType = IteratorRValueReferenceType<IteratorType<R>>;

    template<typename R>
    using RangeDifferenceType = IteratorDifferenceType<IteratorType<R>>;

    template<typename R>
    SSSENGINE_GLOBAL
    constexpr bool EnableSizedRange = true;

    template<typename R>
    concept SizedRangeConcept = EnableSizedRange<R> && RangeConcept<R> && requires(R &r) { Iterators::Count(r); };

    template<typename R>
    SSSENGINE_GLOBAL
    constexpr bool EnableBorrowRange = false;

    template<typename R>
    concept BorrowedRangeConcept =
        RangeConcept<R> && (IsLValueReference<R> || EnableBorrowRange<RemoveCVReferenceType<R>>);

    template<typename R>
    concept InputRangeConcept = RangeConcept<R> && InputIteratorConcept<IteratorType<R>>;

    template<typename R>
    concept MultiPassRangeConcept = InputRangeConcept<R> && MultiPassIteratorConcept<IteratorType<R>>;

    template<typename R>
    concept BidirectionalRangeConcept = MultiPassRangeConcept<R> && BidirectionalIteratorConcept<IteratorType<R>>;

    template<typename R>
    concept RandomAccessRangeConcept = BidirectionalIteratorConcept<R> && RandomAccessIteratorConcept<IteratorType<R>>;

    template<typename R>
    concept ContiguousRangeConcept =
        RandomAccessRangeConcept<R> && ContiguousMemoryIteratorConcept<IteratorType<R>> && requires(T &t) {
            { Iterators::Data(t) } -> SameAsConcept<AddPointerType<RangeReferenceType<R>>>;
        };

} // namespace SSSEngine::Iterators
