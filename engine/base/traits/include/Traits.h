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
 * @brief Some helper traits
 */

#pragma once

#include "ValueConstant.h"

namespace SSSEngine
{
    template<typename T>
    struct SuccessType
    {
        using Type = T;
    };

    struct FailureType
    {
    };

    template<typename>
    struct Protector
    {
        static const bool False = false;
    };

    namespace Impl
    {
        template<typename T, typename RValue = T &&>
        consteval RValue DeclVal(int);

        template<typename T>
        consteval T DeclVal(long);
    } // namespace Impl

    template<typename T>
    consteval auto DeclVal() noexcept -> decltype(Impl::DeclVal<T>(0))
    {
        return Impl::DeclVal<T>(0);
    }

    template<typename T, typename...>
    using FirstType = T;

    template<bool, typename T = void>
    struct EnableChecker
    {
    };

    template<typename T>
    struct EnableChecker<true, T>
    {
        using Type = T;
    };

    template<bool Cond, typename T = void>
    using EnableIf = typename EnableChecker<Cond, T>::Type;

    template<typename... T>
    auto OrFunction(int) -> FirstType<FalseType, EnableIf<!bool(T::value)>...>;

    template<typename... T>
    auto OrFunction(...) -> TrueType;

    template<typename... T>
    auto AndFunction(int) -> FirstType<TrueType, EnableIf<bool(T::value)>...>;

    template<typename... T>
    auto AndFunction(...) -> FalseType;

    template<typename... Args>
    struct Or : decltype(OrFunction<Args...>(0))
    {
    };

    template<typename... Args>
    using OrType = Or<Args...>::Type;

    template<typename... Args>
    struct And : decltype(AndFunction<Args...>(0))
    {
    };

    template<typename... Args>
    using AndType = And<Args...>::Type;

    template<typename...>
    using VoidType = void;

    template<bool, typename T, typename U>
    struct Conditional
    {
        using Type = T;
    };

    template<typename T, typename U>
    struct Conditional<false, T, U>
    {
        using Type = U;
    };

    template<bool Cond, typename T, typename U>
    using ConditionalType = Conditional<Cond, T, U>::Type;

    template<typename... Conditions>
    using Require = EnableIf<And<Conditions...>::Value>;
} // namespace SSSEngine
