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

#include "HelperMacros.h"
#include "Concepts.h"
#include "Attributes.h"
#include "Traits.h"

namespace SSSEngine
{
    namespace SSSENGINE_HIDDEN InternalSwap
    {
        template<typename T>
        void Swap(T &, T &) = delete;

        template<typename T, typename U>
        concept AdlSwapConcept =
            (ClassOrEnumConcept<RemoveReferenceType<T>> || ClassOrEnumConcept<RemoveReferenceType<U>>) &&
            requires(T &&t, U &&u) { Swap(static_cast<T &&>(t), static_cast<U &&>(u)); };

        struct SwapImpl
        {
            template<typename T, typename U>
            static constexpr bool IsNoexcept()
            {
                if constexpr(AdlSwapConcept<T, U>)
                {
                    return noexcept(Swap(DeclVal<T>(), DeclVal<U>()));
                }
                return IsNoThrowMoveConstructible<RemoveReference<T>> && IsNoThrowMoveConstructible<RemoveReference<U>>;
            }

            template<typename T, typename U>
                requires AdlSwapConcept<T, U> ||
                         (IsSameType<T, U> && IsLValueReference<T> && IsMoveConstructible<RemoveReferenceType<T>> &&
                          AssignableFromConcept<T, RemoveReferenceType<T>>)
            constexpr void operator()(T &&t, U &&u) const noexcept(IsNoexcept<T, U>())
            {
                if constexpr(AdlSwapConcept<T, U>)
                {
                    Swap(static_cast<T &&>(t), static_cast<U &&>(u));
                }
                else
                {
                    auto tmp = static_cast<RemoveReferenceType<T> &&>(t);
                    t = static_cast<RemoveReferenceType<T> &&>(u);
                    u = static_cast<RemoveReferenceType<T> &&>(tmp);
                }
            }

            template<typename T, typename U, SizeType Num>
                requires requires(const SwapImpl &swap, T &first, U &second) { swap(first, second); }
            constexpr void operator()(T (&first)[Num], U (&second)[Num]) const
                noexcept(noexcept(DeclVal<const SwapImpl &>()(*first, *second)))
            {
                for(SizeType i = 0; i < Num; ++i)
                {
                    (*this)(first[i], second[i]);
                }
            }
        };
    } // namespace SSSENGINE_HIDDEN InternalSwap

    inline namespace Utility
    {
        SSSENGINE_GLOBAL
        constexpr InternalSwap::SwapImpl Swap{};
    }

    template<typename T>
    concept SwappableConcept = requires(T &a, T &b) { Swap(a, b); };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsSwappable = SwappableConcept<T>;

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowSwappable = InternalSwap::SwapImpl::IsNoexcept<T, T>();

} // namespace SSSEngine
