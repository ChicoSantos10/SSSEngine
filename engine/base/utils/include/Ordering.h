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
#include "Bits.h"
#include "Concepts.h"
#include "HelperMacros.h"
#include "Limits.h"
#include "Math.h"
#include "Traits.h"
#include <compare>

namespace SSSEngine
{
    using StrongOrdering = std::strong_ordering;
    using WeakOrdering = std::weak_ordering;
    using PartialOrdering = std::partial_ordering;

    // NOLINTBEGIN(modernize-use-nullptr)

    SSSENGINE_PURE
    constexpr bool IsEqual(PartialOrdering ordering) noexcept
    {
        return ordering == 0;
    }

    SSSENGINE_PURE
    constexpr bool IsNotEqual(PartialOrdering ordering) noexcept
    {
        return ordering != 0;
    }

    SSSENGINE_PURE
    constexpr bool IsLessThan(PartialOrdering ordering) noexcept
    {
        return ordering < 0;
    }

    SSSENGINE_PURE
    constexpr bool IsGreaterThan(PartialOrdering ordering) noexcept
    {
        return ordering > 0;
    }

    SSSENGINE_PURE
    constexpr bool IsLessEqualThan(PartialOrdering ordering) noexcept
    {
        return ordering <= 0;
    }

    SSSENGINE_PURE
    constexpr bool IsGreaterEqualThan(PartialOrdering ordering) noexcept
    {
        return ordering >= 0;
    }

    // NOLINTEND(modernize-use-nullptr)

    namespace Impl
    {
        using Ordering = std::Impl::Ordering;

        template<typename T>
        SSSENGINE_GLOBAL
        constexpr unsigned CompareId = 1;

        template<>
        SSSENGINE_GLOBAL
        constexpr unsigned CompareId<PartialOrdering> = 2;

        template<>
        SSSENGINE_GLOBAL
        constexpr unsigned CompareId<WeakOrdering> = 4;

        template<>
        SSSENGINE_GLOBAL
        constexpr unsigned CompareId<StrongOrdering> = 8;

        template<typename... Args>
        constexpr auto CommonComparator() noexcept
        {
            constexpr unsigned Cmps = (CompareId<Args> | ...);

            if constexpr(Cmps & 1)
            {
                return;
            }
            if constexpr(SSSEngine::HasBitSet(Cmps, CompareId<PartialOrdering>))
            {
                return PartialOrdering::equivalent;
            }
            else if constexpr(SSSEngine::HasBitSet(Cmps, CompareId<WeakOrdering>))
            {
                return WeakOrdering::equivalent;
            }
            else if constexpr(SSSEngine::HasBitSet(Cmps, CompareId<StrongOrdering>))
            {
                return StrongOrdering::equivalent;
            }
        }
    } // namespace Impl

    template<typename... Args>
    struct CommonComparatorChecker
    {
        using Type = decltype(Impl::CommonComparator<Args...>());
    };

    template<typename T>
    struct CommonComparatorChecker<T>
    {
        using Type = void;
    };

    template<>
    struct CommonComparatorChecker<PartialOrdering>
    {
        using Type = PartialOrdering;
    };

    template<>
    struct CommonComparatorChecker<WeakOrdering>
    {
        using Type = WeakOrdering;
    };

    template<>
    struct CommonComparatorChecker<StrongOrdering>
    {
        using Type = StrongOrdering;
    };

    template<>
    struct CommonComparatorChecker<>
    {
        using Type = StrongOrdering;
    };

    template<typename... Args>
    using CommonComparatorCategory = CommonComparatorChecker<Args...>::Type;

    namespace Impl
    {
        template<typename T, typename Cmp>
        concept ComparesAsConcept = SSSEngine::SameAsConcept<CommonComparatorCategory<T, Cmp>, Cmp>;
    }

    template<typename T, typename Cmp = PartialOrdering>
    concept ThreeWayComparableConcept =
        SSSEngine::EqualityComparableConcept<T> && SSSEngine::OrderableConcept<T> &&
        requires(const SSSEngine::RemoveReferenceType<T> &a, const SSSEngine::RemoveReferenceType<T> &b) {
            { a <=> b } -> Impl::ComparesAsConcept<Cmp>;
        };

    template<typename T, typename U, typename Cmp = PartialOrdering>
    concept ThreeWayComparableWithConcept =
        ThreeWayComparableConcept<T, Cmp> && ThreeWayComparableConcept<U, Cmp> &&
        SSSEngine::ComparisonCommonTypeConcept<T, U> &&
        ThreeWayComparableConcept<
            SSSEngine::CommonReferenceType<const SSSEngine::RemoveReferenceType<T> &, const SSSEngine::RemoveReferenceType<U> &>,
            Cmp> &&
        requires(const SSSEngine::RemoveReferenceType<T> &a, const SSSEngine::RemoveReferenceType<U> &b) {
            { a <=> b } -> Impl::ComparesAsConcept<Cmp>;
            { b <=> a } -> Impl::ComparesAsConcept<Cmp>;
        };

    namespace Impl
    {
        template<typename T, typename U>
        using CmpResultType = decltype(SSSEngine::DeclVal<T>() <=> SSSEngine::DeclVal<U>());

        template<typename T, typename U>
        struct ThreeWayComparisonResultChecker
        {
        };

        template<typename T, typename U>
            requires requires {
                typename CmpResultType<const SSSEngine::RemoveReferenceType<T>, const SSSEngine::RemoveReferenceType<U>>;
            }
        struct ThreeWayComparisonResultChecker<T, U>
        {
            using Type = CmpResultType<const SSSEngine::RemoveReferenceType<T>, const SSSEngine::RemoveReferenceType<U>>;
        };
    } // namespace Impl

    template<typename T, typename U>
    struct ThreeWayComparisonResultChecker : Impl::ThreeWayComparisonResultChecker<T, U>
    {
    };

    template<typename T, typename U>
    using ThreeWayComparisonResultType = ThreeWayComparisonResultChecker<T, U>::Type;

    namespace Impl
    {
        template<typename T, typename U>
      concept NotOverloadedThreeWayComparisonConcept
        =  ! requires(T&& t, U&& u)
	    { operator<=>(static_cast<T&&>(t), static_cast<U&&>(u)); }
	&& ! requires(T&& t, U&& u)
	    { static_cast<T&&>(t).operator<=>(static_cast<U&&>(u)); }
	&& (SSSEngine::SameAsConcept<T, U>
	    || (! requires(T&& t, U&& u)
	         { operator<=>(static_cast<U&&>(u), static_cast<T&&>(t)); }
	       && ! requires(T&& t, U&& u)
	         { static_cast<U&&>(u).operator<=>(static_cast<T&&>(t)); }));

        template<typename T, typename U>
        concept ThreeWayPtrCompareConcept =
            requires(T &&t, U &&u) { static_cast<T &&>(t) <=> static_cast<U &&>(u); } &&
            SSSEngine::ConvertibleToConcept<T, const volatile void *> &&
            SSSEngine::ConvertibleToConcept<U, const volatile void *> && NotOverloadedThreeWayComparisonConcept<T, U>;
    } // namespace Impl

    struct CompareThreeWay
    {
        template<typename T, typename U>
            requires ThreeWayComparableWithConcept<T, U>
        SSSENGINE_PURE
        static constexpr auto
        operator()(T &&t, U &&u) noexcept(noexcept(SSSEngine::DeclVal<T>() <=> SSSEngine::DeclVal<U>()))
        {
            if constexpr(Impl::ThreeWayPtrCompareConcept<T, U>)
            {
                auto ptrT = static_cast<const volatile void *>(t);
                auto ptrU = static_cast<const volatile void *>(u);
                if consteval
                {
                    return ptrT <=> ptrU;
                }
                auto ut = reinterpret_cast<uintptr>(ptrT);
                auto uu = reinterpret_cast<uintptr>(ptrU);
                return ut <=> uu;
            }
            else
            {
                return static_cast<T &&>(t) <=> static_cast<U &&>(u);
            }
        }

        using IsTransparent = void;
    };

    namespace CompareImpl
    {
        template<SSSEngine::FloatingPointConcept N>
        constexpr WeakOrdering FloatingWeakOrdering(N first, N second) noexcept
        {
            auto ordering = Impl::Ordering(first <=> second);
            if(ordering != Impl::Ordering::unordered)
            {
                return std::Impl::Make<WeakOrdering>(ordering);
            }

            auto isNan = [](N v) -> int { return IsNaN(v) ? SSSEngine::Math::SignOf(v) ? -1 : 1 : 0; };

            return isNan(first) <=> isNan(second);
        }

        void StrongOrder() = delete;

        template<typename T, typename U>
        concept AdlStrongConcept =
            requires(T &&t, U &&u) { StrongOrdering(StrongOrder(static_cast<T &&>(t), static_cast<U &&>(u))); };

        void WeakOrder() = delete;

        template<typename T, typename U>
        concept AdlWeakConcept =
            requires(T &&t, U &&u) { WeakOrdering(WeakOrder(static_cast<T &&>(t), static_cast<U &&>(u))); };

        void PartialOrder() = delete;

        template<typename T, typename U>
        concept AdlPartialConcept =
            requires(T &&t, U &&u) { PartialOrdering(PartialOrder(static_cast<T &&>(t), static_cast<U &&>(u))); };

        template<typename Ordering, typename T, typename U>
        concept CmpThreeWayConcept =
            requires(T &&t, U &&u, CompareThreeWay cmp) { Ordering(cmp(static_cast<T &&>(t), static_cast<U &&>(u))); };

        template<typename T, typename U>
        concept DecayedSameAsConcept = SSSEngine::SameAsConcept<SSSEngine::DecayType<T>, SSSEngine::DecayType<U>>;

        template<typename T, typename U>
        concept StronglyOrderedConcept =
            AdlStrongConcept<T, U> || SSSEngine::FloatingPointConcept<SSSEngine::RemoveReferenceType<T>> ||
            CmpThreeWayConcept<StrongOrdering, T, U>;

        class StrongOrderImpl
        {
            template<typename T, typename U>
            static constexpr bool IsNoExcept()
            {
                if constexpr(SSSEngine::FloatingPointConcept<SSSEngine::DecayType<T>>)
                    return true;
                else if constexpr(AdlStrongConcept<T, U>)
                    return noexcept(StrongOrdering(StrongOrderImpl(SSSEngine::DeclVal<T>(), SSSEngine::DeclVal<U>())));
                else if constexpr(CmpThreeWayConcept<StrongOrdering, T, U>)
                    return noexcept(CompareThreeWay()(SSSEngine::DeclVal<T>(), SSSEngine::DeclVal<U>()));
            }

            friend class WeakOrderImpl;
            friend class StrongFallback;

            template<typename T>
            static constexpr T TwosComplement(T t) noexcept
            {
                constexpr auto Bits = SSSEngine::Bits<T>;

                SSSEngine::UnsignedType<T> sign = t >> (Bits - 1);

                return t ^ (sign >> 1);
            }

            template<typename T>
            static constexpr StrongOrdering FloatingPointCompare(T x, T y) noexcept
            {
                auto xBits = SSSEngine::AsBits(x);
                auto yBits = SSSEngine::AsBits(y);

                if(xBits == yBits)
                {
                    return StrongOrdering::equal;
                }

                TwosComplement(x) <=> TwosComplement(y);
            }

          public:
            template<typename T, DecayedSameAsConcept<T> U>
                requires StronglyOrderedConcept<T, U>
            SSSENGINE_PURE
            constexpr StrongOrdering operator()(T &&t, U &&u) const noexcept(IsNoExcept<T, U>())
            {
                if constexpr(SSSEngine::IsFloatingPoint<SSSEngine::DecayType<T>>)
                {
                    return FloatingPointCompare(t, u);
                }
                else if constexpr(AdlStrongConcept<T, U>)
                {
                    return StrongOrdering(StrongOrderImpl(static_cast<T &&>(t), StrongOrderImpl(static_cast<U &&>(u))));
                }
                else if constexpr(CmpThreeWayConcept<StrongOrdering, T, U>)
                {
                    return CompareThreeWay()(static_cast<T &&>(t), static_cast<U &&>(u));
                }
            }
        };

        template<typename T, typename U>
        concept WeaklyOrderedConcept =
            AdlWeakConcept<T, U> || SSSEngine::FloatingPointConcept<SSSEngine::RemoveReferenceType<T>> ||
            CmpThreeWayConcept<WeakOrdering, T, U> || StronglyOrderedConcept<T, U>;

        class WeakOrderImpl
        {
            template<typename T, typename U>
            static constexpr bool IsNoExcept()
            {
                if constexpr(SSSEngine::FloatingPointConcept<SSSEngine::DecayType<T>>)
                    return true;
                else if constexpr(AdlWeakConcept<T, U>)
                    return noexcept(WeakOrdering(WeakOrderImpl(SSSEngine::DeclVal<T>(), SSSEngine::DeclVal<U>())));
                else if constexpr(CmpThreeWayConcept<WeakOrdering, T, U>)
                    return noexcept(CompareThreeWay()(SSSEngine::DeclVal<T>(), SSSEngine::DeclVal<U>()));
                else if constexpr(StronglyOrderedConcept<T, U>)
                    return StrongOrderImpl::IsNoExcept<T, U>();
            }

            friend class PartialOrderImpl;
            friend class WeakFallback;

          public:
            template<typename T, DecayedSameAsConcept<T> U>
                requires WeaklyOrderedConcept<T, U>
            SSSENGINE_PURE
            constexpr WeakOrdering operator()(T &&t, U &&u) const noexcept(IsNoExcept<T, U>())
            {
                if constexpr(SSSEngine::IsFloatingPoint<SSSEngine::DecayType<T>>)
                {
                    return FloatingWeakOrdering(t, u);
                }
                else if constexpr(AdlWeakConcept<T, U>)
                {
                    return WeakOrdering(WeakOrderImpl(static_cast<T &&>(t), WeakOrderImpl(static_cast<U &&>(u))));
                }
                else if constexpr(CmpThreeWayConcept<WeakOrdering, T, U>)
                {
                    return CompareThreeWay()(static_cast<T &&>(t), static_cast<U &&>(u));
                }
                else if constexpr(StronglyOrderedConcept<T, U>)
                {
                    return StrongOrderImpl{}(static_cast<T &&>(t), static_cast<U &&>(u));
                }
            }
        };

        template<typename T, typename U>
        concept PartiallyOrderedConcept =
            AdlPartialConcept<T, U> || CmpThreeWayConcept<PartialOrdering, T, U> || WeaklyOrderedConcept<T, U>;

        class PartialOrderImpl
        {
            template<typename T, typename U>
            static constexpr bool IsNoExcept()
            {
                if constexpr(AdlPartialConcept<T, U>)
                    return noexcept(PartialOrdering(PartialOrderImpl(SSSEngine::DeclVal<T>(), SSSEngine::DeclVal<U>())));
                else if constexpr(CmpThreeWayConcept<PartialOrdering, T, U>)
                    return noexcept(CompareThreeWay()(SSSEngine::DeclVal<T>(), SSSEngine::DeclVal<U>()));
                else if constexpr(WeaklyOrderedConcept<T, U>)
                    return WeakOrderImpl::IsNoExcept<T, U>();
            }

            friend class PartialFallback;

          public:
            template<typename T, DecayedSameAsConcept<T> U>
                requires PartiallyOrderedConcept<T, U>
            SSSENGINE_PURE
            constexpr PartialOrdering operator()(T &&t, U &&u) const noexcept(IsNoExcept<T, U>())
            {
                if constexpr(AdlPartialConcept<T, U>)
                {
                    return PartialOrdering(PartialOrderImpl(static_cast<T &&>(t), PartialOrderImpl(static_cast<U &&>(u))));
                }
                else if constexpr(CmpThreeWayConcept<PartialOrdering, T, U>)
                {
                    return CompareThreeWay()(static_cast<T &&>(t), static_cast<U &&>(u));
                }
                else if constexpr(WeaklyOrderedConcept<T, U>)
                {
                    return WeakOrderImpl{}(static_cast<T &&>(t), static_cast<U &&>(u));
                }
            }
        };

        template<typename T, typename U>
        concept EqLtConcept = requires(T &&t, U &&u) {
            { static_cast<T &&>(t) == static_cast<U &&>(u) } -> SSSEngine::ConvertibleToConcept<bool>;
            { static_cast<T &&>(t) < static_cast<U &&>(u) } -> SSSEngine::ConvertibleToConcept<bool>;
        };

        class StrongFallback
        {
          private:
            template<typename T, typename U>
            constexpr static bool IsNoExcept()
            {
                if constexpr(StronglyOrderedConcept<T, U>)
                {
                    return StrongOrderImpl::IsNoExcept<T, U>();
                }
                else
                {
                    return noexcept(bool(SSSEngine::DeclVal<T>() == SSSEngine::DeclVal<U>())) &&
                           noexcept(bool(SSSEngine::DeclVal<T>() < SSSEngine::DeclVal<U>()));
                }
            }

          public:
            template<typename T, DecayedSameAsConcept<T> U>
                requires StronglyOrderedConcept<T, U> || EqLtConcept<T, U>
            SSSENGINE_PURE
            constexpr StrongOrdering operator()(T &&t, U &&u) const noexcept(IsNoExcept<T, U>())
            {
                if constexpr(StronglyOrderedConcept<T, U>)
                {
                    return StrongOrderImpl{}(static_cast<T &&>(t), static_cast<U &&>(u));
                }
                else
                {
                    return static_cast<T &&>(t) == static_cast<U &&>(u) ? StrongOrdering::equal :
                           static_cast<T &&>(t) < static_cast<U &&>(u)  ? StrongOrdering::less :
                                                                          StrongOrdering::greater;
                }
            }
        };

        class WeakFallback
        {
          private:
            template<typename T, typename U>
            constexpr static bool IsNoExcept()
            {
                if constexpr(WeaklyOrderedConcept<T, U>)
                {
                    return WeakOrderImpl::IsNoExcept<T, U>();
                }
                else
                {
                    return noexcept(bool(SSSEngine::DeclVal<T>() == SSSEngine::DeclVal<U>())) &&
                           noexcept(bool(SSSEngine::DeclVal<T>() < SSSEngine::DeclVal<U>()));
                }
            }

          public:
            template<typename T, DecayedSameAsConcept<T> U>
                requires WeaklyOrderedConcept<T, U> || EqLtConcept<T, U>
            SSSENGINE_PURE
            constexpr WeakOrdering operator()(T &&t, U &&u) const noexcept(IsNoExcept<T, U>())
            {
                if constexpr(WeaklyOrderedConcept<T, U>)
                {
                    return WeakOrderImpl{}(static_cast<T &&>(t), static_cast<U &&>(u));
                }
                else
                {
                    return static_cast<T &&>(t) == static_cast<U &&>(u) ? WeakOrdering::equivalent :
                           static_cast<T &&>(t) < static_cast<U &&>(u)  ? WeakOrdering::less :
                                                                          WeakOrdering::greater;
                }
            }
        };

        template<typename T, typename U>
        concept EqLtBothWaysConcept = EqLtConcept<T, U> && requires(T &&t, U &&u) {
            { static_cast<T &&>(u) < static_cast<U &&>(u) } -> SSSEngine::ConvertibleToConcept<bool>;
        };

        class PartialFallback
        {
          private:
            template<typename T, typename U>
            constexpr static bool IsNoExcept()
            {
                if constexpr(PartiallyOrderedConcept<T, U>)
                {
                    return PartialOrderImpl::IsNoExcept<T, U>();
                }
                else
                {
                    return noexcept(bool(SSSEngine::DeclVal<T>() == SSSEngine::DeclVal<U>())) &&
                           noexcept(bool(SSSEngine::DeclVal<T>() < SSSEngine::DeclVal<U>()));
                }
            }

          public:
            template<typename T, DecayedSameAsConcept<T> U>
                requires PartiallyOrderedConcept<T, U> || EqLtBothWaysConcept<T, U>
            SSSENGINE_PURE
            constexpr PartialOrdering operator()(T &&t, U &&u) const noexcept(IsNoExcept<T, U>())
            {
                if constexpr(PartiallyOrderedConcept<T, U>)
                {
                    return PartialOrderImpl{}(static_cast<T &&>(t), static_cast<U &&>(u));
                }
                else
                {
                    return static_cast<T &&>(t) == static_cast<U &&>(u) ? PartialOrdering::equivalent :
                           static_cast<T &&>(t) < static_cast<U &&>(u)  ? PartialOrdering::less :
                           static_cast<T &&>(u) < static_cast<U &&>(t)  ? PartialOrdering::greater :
                                                                          PartialOrdering::unordered;
                }
            }
        };

    } // namespace CompareImpl

    inline constexpr struct
    {
        template<typename T, typename U>
        static constexpr bool IsNoExcept(const T *t = nullptr, const U *u = nullptr)
        {
            if constexpr(ThreeWayComparableWithConcept<T, U, PartialOrdering>)
                return noexcept(*t <=> *u);
            else
                return noexcept(*t < *u) && noexcept(*u < *t);
        }

        template<typename T, typename U>
            SSSENGINE_PURE
        constexpr auto operator()(const T &t, const U &u) const noexcept(IsNoExcept<T, U>())
            requires requires {
                { t < u } -> SSSEngine::BooleanTestableConcept;
                { u < t } -> SSSEngine::BooleanTestableConcept;
            }
        {
            if constexpr(ThreeWayComparableWithConcept<T, U>)
                return t <=> u;
            else
            {
                if(t < u)
                {
                    return WeakOrdering::less;
                }

                if(u < t)
                {
                    return WeakOrdering::greater;
                }

                return WeakOrdering::equivalent;
            }
        }
    } SynthThreeWay = {};

    template<typename T, typename U = T>
    using SynthThreeWayType = decltype(SynthThreeWay(SSSEngine::DeclVal<T &>(), SSSEngine::DeclVal<U &>()));

    // namespace Impl

    inline namespace Compare
    {
        SSSENGINE_GLOBAL
        constexpr CompareImpl::StrongOrderImpl StrongOrder{};

        SSSENGINE_GLOBAL
        constexpr CompareImpl::WeakOrderImpl WeakOrder{};

        SSSENGINE_GLOBAL
        constexpr CompareImpl::PartialOrderImpl PartialOrder{};

        SSSENGINE_GLOBAL
        constexpr CompareImpl::StrongFallback CompareStrongOrderFallback{};

        SSSENGINE_GLOBAL
        constexpr CompareImpl::WeakFallback CompareWeakOrderFallback{};

        SSSENGINE_GLOBAL
        constexpr CompareImpl::PartialFallback ComparePartialOrderFallback{};

    } // namespace Compare

} // namespace SSSEngine
