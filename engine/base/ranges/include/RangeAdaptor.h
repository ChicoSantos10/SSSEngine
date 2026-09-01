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
#include "Bind.h"
#include "Concepts.h"
#include "CopyAndMoveTraits.h"
#include "QualifierTraits.h"
#include "Traits.h"
#include "Utility.h"

namespace SSSEngine::Ranges
{
    template<typename Derived>
    struct RangeAdaptorClosure;

    template<typename Derived>
    struct RangeAdaptor;

    template<typename Adaptor, typename... Args>
    struct Partial;

    template<typename Lhs, typename Rhs>
    struct Pipe;

    // NOLINTBEGIN(bugprone-reserved-identifier, readability-identifier-naming)

    namespace __impl
    {
        template<typename Adaptor, typename... Args>
        concept __AdaptorInvocableConcept = requires { DeclVal<Adaptor>()(DeclVal<Args>()...); };

        template<typename Adaptor, typename... Args>
        concept __AdaptorPartialAppliableConcept = (Adaptor::Arity > 1) && (sizeof...(Args) == Adaptor::Arity - 1) &&
                                                   (ConstructibleFromConcept<DecayType<Args>, Args> && ...);
        template<typename T, typename U>
            requires(!SameAsConcept<T, RangeAdaptorClosure<U>>)
        void __IsRangeAdaptorClosureFn(const T &, const RangeAdaptorClosure<U> &);

        template<typename T>
        concept __IsRangeAdaptorClosureConcept = requires(T t) { __IsRangeAdaptorClosureFn(t, t); };

        template<typename Adaptor>
        concept __ClosureHasSimpleCallConcept = Adaptor::SimpleCallOp;

        template<typename Adaptor, typename... Args>
        concept __AdaptorHasSimpleExtraArgsConcept =
            Adaptor::HasSimpleExtraArgs || Adaptor::template HasSimpleExtraArgs<Args...>;

        template<typename Lhs, typename Rhs, typename Range>
        concept __PipeInvocableConcept = requires { DeclVal<Rhs>()(DeclVal<Lhs>()(DeclVal<Range>())); };
    } // namespace __impl

    // NOLINTEND(bugprone-reserved-identifier, readability-identifier-naming)

    template<typename Self, typename Range>
        requires __impl::__IsRangeAdaptorClosureConcept<Self> && __impl::__AdaptorInvocableConcept<Self, Range>
    constexpr auto operator|(Range &&range, Self &&self)
    {
        return Forward<Self>(self)(Forward<Range>(range));
    }

    template<typename Lhs, typename Rhs>
        requires __impl::__IsRangeAdaptorClosureConcept<Lhs> && __impl::__IsRangeAdaptorClosureConcept<Rhs>
    constexpr auto operator|(Lhs &&lhs, Rhs &&rhs)
    {
        return Pipe<DecayType<Lhs>, DecayType<Rhs>>{Forward<Lhs>(lhs), Forward<Rhs>(rhs)};
    }

    template<typename Derived>
    struct RangeAdaptorClosure
    {
        template<typename Self, typename Range>
            requires __impl::__IsRangeAdaptorClosureConcept<Self> && __impl::__AdaptorInvocableConcept<Self, Range>
        friend constexpr auto operator|(Range &&range, Self &&self);

        template<typename Lhs, typename Rhs>
            requires __impl::__IsRangeAdaptorClosureConcept<Lhs> && __impl::__IsRangeAdaptorClosureConcept<Rhs>
        friend constexpr auto operator|(Lhs &&lhs, Rhs &&rhs);
    };

    template<typename Derived>
    struct RangeAdaptor
    {
        template<typename... Args>
            requires __impl::__AdaptorPartialAppliableConcept<Derived, Args...>
        constexpr auto operator()(Args &&...args) const
        {
            return Partial<Derived, DecayType<Args>...>{0, Forward<Args>(args)...};
        }
    };

    template<typename Adaptor, typename... Args>
    struct Partial : RangeAdaptorClosure<Partial<Adaptor, Args...>>
    {
        using BinderType = BindBackType<Adaptor, Args...>;
        SSSENGINE_OVERLAP BinderType binder;

        template<typename... Types>
        constexpr Partial(InPlaceType, Types &&...args) : binder(InPlace, Forward<Types>(args)...)
        {
        }

        template<typename Self, typename Range>
            requires __impl::__AdaptorInvocableConcept<Adaptor, Range, LikeType<Self, Args>...>
        constexpr auto operator()(this Self &&self, Range &&range)
        {
            return BinderType::Call(LikeType<Self, Partial>(self).binder, Forward<Range>(range));
        }
    };

    template<typename Adaptor, typename... Args>
        requires __impl::__AdaptorHasSimpleExtraArgsConcept<Adaptor, Args...> && (IsTriviallyCopyConstructible<Args> && ...)
    struct Partial<Adaptor, Args...> : RangeAdaptorClosure<Partial<Adaptor, Args...>>
    {
        using BinderType = BindBackType<Adaptor, Args...>;
        SSSENGINE_OVERLAP
        BinderType binder;

        template<typename... Types>
        constexpr Partial(InPlaceType, Types &&...args) : binder(InPlace, Adaptor(), Forward<Types>(args)...)
        {
        }

        // Invoke Adaptor with arguments __r, const _M_args&... regardless
        // of the value category of this _Partial object.
        template<typename Range>
            requires __impl::__AdaptorInvocableConcept<Adaptor, Range, const Args &...>
        constexpr auto operator()(Range &&range) const
        {
            return BinderType::Call(binder, Forward<Range>(range));
        }

        static constexpr bool SimpleCallOp = true;
    };

    template<typename Lhs, typename Rhs>
    struct Pipe : RangeAdaptorClosure<Pipe<Lhs, Rhs>>
    {
        SSSENGINE_OVERLAP Lhs lhs;
        SSSENGINE_OVERLAP Rhs rhs;

        template<typename T, typename U>
        constexpr Pipe(T &&lhs, U &&rhs) : lhs(Forward<Lhs>(lhs)), rhs(Forward<Rhs>(rhs))
        {
        }

        template<typename Self, typename Range>
            requires __impl::__PipeInvocableConcept<LikeType<Self, Lhs>, LikeType<Self, Rhs>, Range>
        constexpr auto operator()(this Self &&self, Range &&range)
        {
            return (LikeType<Self, Pipe>(self).rhs(LikeType<Self, Pipe>(self).lhs(Forward<Range>(range))));
        }
    };

    template<typename Lhs, typename Rhs>
        requires __impl::__ClosureHasSimpleCallConcept<Lhs> && __impl::__ClosureHasSimpleCallConcept<Rhs>
    struct Pipe<Lhs, Rhs> : RangeAdaptorClosure<Pipe<Lhs, Rhs>>
    {
        SSSENGINE_OVERLAP
        Lhs lhs;
        SSSENGINE_OVERLAP
        Rhs rhs;

        template<typename T, typename U>
        constexpr Pipe(T &&lhs, U &&rhs) : lhs(Forward<T>(lhs)), rhs(Forward<U>(rhs))
        {
        }

        template<typename Range>
            requires __impl::__PipeInvocableConcept<const Lhs &, const Rhs &, Range>
        constexpr auto operator()(Range &&range) const
        {
            return rhs(lhs(Forward<Range>(range)));
        }

        static constexpr bool SimpleCallOp = true;
    };

} // namespace SSSEngine::Ranges
