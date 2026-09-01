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
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "Invoke.h"
#include "Utility.h"

namespace SSSEngine
{
    // NOLINTBEGIN(readability-identifier-naming, bugprone-reserved-identifier)
    namespace __impl
    {
        template<SizeType, typename T>
        struct _IndexedBoundArg
        {
            SSSENGINE_OVERLAP T value;
        };

        template<typename... IndexedArgs>
        struct _BoundArgStorage : IndexedArgs...
        {
            template<bool Back, typename F, typename Self, typename... CallArgs>
            static constexpr decltype(auto) Apply(F &&fn, Self &&self, CallArgs &&...callArgs)
            {
                if constexpr(Back)
                    return Invoke(Forward<F>(fn), Forward<CallArgs>(callArgs)..., LikeType<Self, IndexedArgs>(self).value...);
                else
                    return Invoke(Forward<F>(fn), LikeType<Self, IndexedArgs>(self).value..., Forward<CallArgs>(callArgs)...);
            }
        };

        template<typename... BoundArgs, typename... Args>
        constexpr auto __MakeBoundArgs(Args &&...args)
        {
            if constexpr(sizeof...(BoundArgs) == 1)
            {
                return (BoundArgs(Forward<Args>(args)), ...);
            }
            else
            {
                auto impl = [&]<SizeType... Indices>(IndexSequence<Indices...>)
                {
                    return _BoundArgStorage<_IndexedBoundArg<Indices, BoundArgs>...>{{BoundArgs(Forward<Args>(args))}...};
                };
                return impl(index_sequence_for<BoundArgs...>());
            }
        }

        template<bool Back, MoveConstructibleConcept F, MoveConstructibleConcept... BoundArgs>
        struct _Binder
        {
            template<typename Self, typename... CallArgs>
            using ResultType =
                ConditionalType<Back,
                                InvokeResult<LikeType<Self, F>, CallArgs..., LikeType<Self, BoundArgs>...>,
                                InvokeResult<LikeType<Self, F>, LikeType<Self, BoundArgs>..., CallArgs...>>::Type;

            template<typename Self, typename... CallArgs>
            static consteval bool IsNoExceptInvocable()
            {
                if constexpr(Back)
                    return IsNoThrowInvocable<LikeType<Self, F>, CallArgs..., LikeType<Self, BoundArgs>...>;
                else
                    return IsNoThrowInvocable<LikeType<Self, F>, LikeType<Self, BoundArgs>..., CallArgs...>;
            }

          public:
            template<typename Func, typename... Args>
            explicit constexpr _Binder(InPlaceType, Func &&fn, Args &&...args) noexcept(
                And<NoThrowConstructibleChecker<F, F>, NoThrowConstructibleChecker<BoundArgs, Args>...>::Value) :
                fn(Forward<Func>(fn)), boundArgs(__MakeBoundArgs<BoundArgs...>(Forward<Args>(args)...))
            {
                SSSENGINE_STATIC_ASSERT(sizeof...(Args) == sizeof...(BoundArgs));
            }

            template<typename Self, typename... CallArgs>
            constexpr ResultType<Self, CallArgs...>
            operator()(this Self &&self, CallArgs &&...callArgs) noexcept(IsNoExceptInvocable<Self, CallArgs...>())
            {
                return Call(LikeType<Self, _Binder>(self), Forward<CallArgs>(callArgs)...);
            }

            template<typename T, typename... CallArgs>
            static constexpr decltype(auto) Call(T &&g, CallArgs &&...callArgs)
            {
                if constexpr(sizeof...(BoundArgs) > 1)
                {
                    return _BoundArgsStorage::template Apply<Back>(
                        Forward<T>(g).fn, Forward<T>(g).boundArgs, Forward<CallArgs>(callArgs)...);
                }
                else if constexpr(sizeof...(BoundArgs) == 0)
                {
                    return Invoke(Forward<T>(g).fn, Forward<CallArgs>(callArgs)...);
                }
                else if constexpr(Back) // sizeof...(BoundArgs) == 1
                {
                    return Invoke(Forward<T>(g).fn, Forward<CallArgs>(callArgs)..., Forward<T>(g).boundArgs);
                }
                else
                {
                    return Invoke(Forward<T>(g).fn, Forward<T>(g).boundArgs, Forward<CallArgs>(callArgs)...);
                }
            }

          private:
            using _BoundArgsStorage = decltype(__MakeBoundArgs<BoundArgs...>(DeclVal<BoundArgs>()...));

            SSSENGINE_OVERLAP F fn;
            SSSENGINE_OVERLAP _BoundArgsStorage boundArgs;
        };

    } // namespace __impl

    // NOLINTEND(readability-identifier-naming, bugprone-reserved-identifier)

    template<typename F, typename... Args>
    using BindFrontType = __impl::_Binder<false, DecayType<F>, DecayType<Args>...>;

    template<typename F, typename... Args>
    using BindBackType = __impl::_Binder<(sizeof...(Args) > 0), DecayType<F>, DecayType<Args>...>;

} // namespace SSSEngine
