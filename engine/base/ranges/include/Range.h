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
#include "ConstIterator.h"
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "Iterator.h"
#include "Limits.h"
#include "QualifierTraits.h"
#include "SignTraits.h"
#include "Traits.h"
#include "ReverseIterator.h"
#include "Types.h"

namespace SSSEngine::Ranges
{
    template<typename R>
    SSSENGINE_GLOBAL
    constexpr bool EnableBorrowRange = false;

    namespace __impl // NOLINT(readability-identifier-naming, bugprone-reserved-identifier)
    {
        template<typename R>
        concept CanBorrowRangeConcept = (IsLValueReference<R> || EnableBorrowRange<RemoveCVReferenceType<R>>);

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
        concept HasReverseBeginConcept = requires(T &t) {
            { DecayType<decltype(t.ReverseBegin())>(t.ReverseBegin()) } -> IteratorConcept;
        };

        void ReverseBegin() = delete;

        template<typename T>
        concept AdlReverseBeginConcept = requires(T &t) {
            { DecayType<decltype(ReverseBegin(t))>(ReverseBegin(t)) } -> IteratorConcept;
        };

        template<typename T>
        concept ReversableConcept = requires(T &t) {
            { BeginImpl{}(t) } -> BidirectionalIteratorConcept;
            { EndImpl{}(t) } -> SameAsConcept<decltype(BeginImpl{}(t))>;
        };

        class ReverseBeginImpl
        {
          private:
            template<typename T>
            static consteval bool IsNoExcept()
            {
                if constexpr(HasReverseBeginConcept<T>)
                {
                    return noexcept(DecayType<decltype(DeclVal<T>().ReverseBegin())>(DeclVal<T>().ReverseBegin()));
                }
                else if constexpr(AdlReverseBeginConcept<T>)
                {
                    return noexcept(DecayType<decltype(ReverseBegin(DeclVal<T>()))>(ReverseBegin(DeclVal<T>())));
                }
                else
                {
                    if constexpr(noexcept(EndImpl{}(DeclVal<T &>())))
                    {
                        using It = decltype(EndImpl{}(DeclVal<T &>()));
                        return IsNoThrowCopyConstructible<It>;
                    }
                    else
                    {
                        return false;
                    }
                }
            }

          public:
            template<CanBorrowRangeConcept T>
                requires HasReverseBeginConcept<T> || AdlReverseBeginConcept<T> || ReversableConcept<T>
                SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&range) const noexcept(IsNoExcept<T>())
            {
                if constexpr(HasReverseBeginConcept<T>)
                {
                    return range.ReverseBegin();
                }
                else if constexpr(AdlReverseBeginConcept<T>)
                {
                    return ReverseBegin(range);
                }
                else
                {
                    MakeReverseIterator(EndImpl{}(range));
                }
            }
        };

        template<typename T>
        concept HasReverseEndConcept = requires(T &t) {
            {
                DecayType<decltype(t.ReverseEnd())>(t.ReverseEnd())
            } -> SentinelForConcept<decltype(ReverseBeginImpl{}(Forward<T>(t)))>;
        };

        void ReverseEnd() = delete;

        template<typename T>
        concept AdlReverseEndConcept = requires(T &t) {
            { DecayType<decltype(ReverseEnd(t))>(ReverseEnd(t)) } -> IteratorConcept;
        };

        class ReverseEndImpl
        {
          private:
            template<typename T>
            static consteval bool IsNoExcept()
            {
                if constexpr(HasReverseEndConcept<T>)
                {
                    return noexcept(DecayType<decltype(DeclVal<T>().ReverseEnd())>(DeclVal<T>().ReverseEnd()));
                }
                else if constexpr(AdlReverseEndConcept<T>)
                {
                    return noexcept(DecayType<decltype(ReverseEnd(DeclVal<T>()))>(ReverseEnd(DeclVal<T>())));
                }
                else
                {
                    if constexpr(noexcept(BeginImpl{}(DeclVal<T &>())))
                    {
                        using It = decltype(BeginImpl{}(DeclVal<T &>()));
                        return IsNoThrowCopyConstructible<It>;
                    }
                    else
                    {
                        return false;
                    }
                }
            }

          public:
            template<CanBorrowRangeConcept T>
                requires HasReverseEndConcept<T> || AdlReverseEndConcept<T> || ReversableConcept<T>
                SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&range) const noexcept(IsNoExcept<T>())
            {
                if constexpr(HasReverseEndConcept<T>)
                {
                    return range.ReverseEnd();
                }
                else if constexpr(AdlReverseEndConcept<T>)
                {
                    return ReverseEnd(range);
                }
                else
                {
                    MakeReverseIterator(BeginImpl{}(range));
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

        class SignedCountImpl
        {
          public:
            template<typename T>
                requires requires(T &t) { CountImpl{}(t); }
            SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&t) const noexcept(noexcept(CountImpl{}(t)))
            {
                auto count = CountImpl{}(t);
                using CountType = decltype(count);
                if constexpr(IntegralConcept<CountType>)
                {
                    using Math::Limits::BinaryDigits;
                    if constexpr(BinaryDigits<CountType> < BinaryDigits<ptrdiff>)
                    {
                        return static_cast<ptrdiff>(count);
                    }
                    else
                    {
                        return static_cast<SignedType<CountType>>(count);
                    }
                }
                else
                {
                    SSSENGINE_NOT_IMPLEMENTED;
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
            template<CanBorrowRangeConcept T>
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

        template<typename T>
        concept HasIsEmptyConcept = requires(T &t) {
            { t.empty() } -> ConvertibleToConcept<bool>;
        };

        template<typename T>
        concept CheckCountIsZeroConcept = requires(T &t) { CountImpl{}(t) == 0; };

        template<typename T>
        concept CheckIteratorConcept = requires(T &t) {
            requires(!IsArrayUnknownBounds<RemoveReferenceType<T>>);

            { BeginImpl{}(t) } -> MultiPassIteratorConcept;
            { BeginImpl{}(t) == EndImpl{}(t) } -> ConvertibleToConcept<bool>;
        };

        struct IsEmptyImpl
        {
          private:
            template<typename T>
            static consteval bool IsNoExcept()
            {
                if constexpr(HasIsEmptyConcept<T>)
                    return noexcept(bool(DeclVal<T &>().empty()));
                else if constexpr(CheckCountIsZeroConcept<T>)
                    return noexcept(CountImpl{}(DeclVal<T &>()) == 0);
                else
                    return noexcept(bool(BeginImpl{}(DeclVal<T &>()) == EndImpl{}(DeclVal<T &>())));
            }

          public:
            template<typename T>
                requires HasIsEmptyConcept<T> || CheckCountIsZeroConcept<T> || CheckIteratorConcept<T>
                SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr bool operator()(T &&t) const noexcept(IsNoExcept<T &>())
            {
                if constexpr(HasIsEmptyConcept<T>)
                    return bool(t.empty());
                else if constexpr(CheckCountIsZeroConcept<T>)
                    return CountImpl{}(t) == 0;
                else
                    return bool(BeginImpl{}(t) == EndImpl{}(t));
            }
        };

    } // namespace __impl

    inline namespace Utility
    {
        SSSENGINE_GLOBAL
        constexpr __impl::BeginImpl Begin{};

        SSSENGINE_GLOBAL
        constexpr __impl::EndImpl End{};

        SSSENGINE_GLOBAL
        constexpr __impl::ReverseBeginImpl ReverseBegin{};

        SSSENGINE_GLOBAL
        constexpr __impl::ReverseEndImpl ReverseEnd{};

        SSSENGINE_GLOBAL
        constexpr __impl::CountImpl Count{};

        SSSENGINE_GLOBAL
        constexpr __impl::SignedCountImpl SignedCount{};

        SSSENGINE_GLOBAL
        constexpr __impl::DataImpl Data{};

        SSSENGINE_GLOBAL
        constexpr __impl::IsEmptyImpl IsEmpty{};

    } // namespace Utility

    template<typename R>
    concept RangeConcept = requires(R &r) {
        Ranges::Begin(r);
        Ranges::End(r);
    };

    template<typename R>
    using IteratorType = __impl::RangeIteratorType<R>;

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
    concept SizedRangeConcept = EnableSizedRange<R> && RangeConcept<R> && requires(R &r) { Ranges::Count(r); };

    template<typename R>
    concept BorrowedRangeConcept = RangeConcept<R> && __impl::CanBorrowRangeConcept<R>;

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
        RandomAccessRangeConcept<R> && ContiguousMemoryIteratorConcept<IteratorType<R>> && requires(R &t) {
            { Ranges::Data(t) } -> SameAsConcept<AddPointerType<RangeReferenceType<R>>>;
        };

    namespace __impl
    {
        template<InputRangeConcept Range>
            SSSENGINE_FORCE_INLINE
        constexpr auto &PossiblyConstRange(Range &range) noexcept
        {
            if constexpr(InputRangeConcept<const Range>)
            {
                return const_cast<const Range &>(range);
            }
            else
            {
                return range;
            }
        }

        class ConstBeginImpl
        {
            template<CanBorrowRangeConcept T>
                SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&t) const noexcept(noexcept(MakeConstIterator(Begin(PossiblyConstRange(t)))))
                requires requires { MakeConstIterator(Begin(PossiblyConstRange(t))); }
            {
                auto &range = PossiblyConstRange(t);
                return ConstIterator<decltype(Begin(range))>(Begin(range));
            }
        };

        class ConstEndImpl
        {
            template<CanBorrowRangeConcept T>
                SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&t) noexcept(noexcept(MakeConstSentinel(End(PossiblyConstRange(t)))))
                requires requires { MakeConstSentinel(End(PossiblyConstRange(t))); }
            {
                auto &range = PossiblyConstRange(t);
                return ConstIterator<decltype(End(range))>(End(range));
            }
        };

        class ConstReverseBeginImpl
        {
            template<CanBorrowRangeConcept T>
                SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&t) const
                noexcept(noexcept(MakeConstIterator(ReverseBegin(PossiblyConstRange(t)))))
                requires requires { MakeConstIterator(ReverseBegin(PossiblyConstRange(t))); }
            {
                auto &range = PossiblyConstRange(t);
                return ConstIterator<decltype(ReverseBegin(range))>(ReverseBegin(range));
            }
        };

        class ConstReverseEndImpl
        {
            template<CanBorrowRangeConcept T>
                SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&t) noexcept(noexcept(MakeConstSentinel(ReverseEnd(PossiblyConstRange(t)))))
                requires requires { MakeConstSentinel(ReverseEnd(PossiblyConstRange(t))); }
            {
                auto &range = PossiblyConstRange(t);
                return ConstIterator<decltype(ReverseEnd(range))>(ReverseEnd(range));
            }
        };

        class ConstDataImpl
        {
            template<CanBorrowRangeConcept T>
                SSSENGINE_PURE SSSENGINE_FORCE_INLINE
            constexpr auto operator()(T &&t) noexcept(noexcept(Ranges::Data(PossiblyConstRange(t))))
                requires requires { Ranges::Data(PossiblyConstRange(t)); }
            {
                return Ranges::Data(PossiblyConstRange(t));
            }
        };

    } // namespace __impl

    inline namespace Utility
    {
        SSSENGINE_GLOBAL
        constexpr __impl::ConstBeginImpl ConstBegin{};

        SSSENGINE_GLOBAL
        constexpr __impl::ConstEndImpl ConstEnd{};

        SSSENGINE_GLOBAL
        constexpr __impl::ConstReverseBeginImpl ConstReverseBegin{};

        SSSENGINE_GLOBAL
        constexpr __impl::ConstReverseEndImpl ConstReverseEnd{};

        SSSENGINE_GLOBAL
        constexpr __impl::ConstDataImpl ConstData{};

    } // namespace Utility

} // namespace SSSEngine::Ranges
