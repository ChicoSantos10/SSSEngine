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
 * @brief Traits regarding copying and moving
 */

#pragma once

#include "ArrayTraits.h"
#include "HelperMacros.h"
#include "QualifierTraits.h"
#include "Traits.h"
#include "ValueConstant.h"

namespace SSSEngine
{
    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsTriviallyDefaultConstructible = __is_trivially_constructible(T);

    template<typename T>
        requires(IsCompleteOrUnbounded<T>)
    struct TriviallyCopyableChecker : public BoolConstant<__is_trivially_copyable(T)>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsTriviallyCopyable = TriviallyCopyableChecker<T>::Value;

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsTriviallyCopyConstructible = __is_trivially_constructible(T, AddLValueRefType<const T>);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsTriviallyMoveConstructible = __is_trivially_constructible(T, AddRValueRefType<T>);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsTriviallyCopyAssignable = __is_trivially_assignable(AddLValueRefType<T>, AddLValueRefType<const T>);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsTriviallyMoveAssignable = __is_trivially_assignable(AddLValueRefType<T>, AddRValueRefType<T>);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsBitwiseCopyable = __is_bitwise_cloneable(T);

    template<typename T, typename... Args>
    struct NoThrowAssignableChecker : BoolConstant<__is_nothrow_assignable(T, Args...)>
    {
    };

    template<typename T, typename... Args>
    struct NoThrowConstructibleChecker : BoolConstant<__is_nothrow_constructible(T, Args...)>
    {
    };

    template<typename T>
        requires(IsCompleteOrUnbounded<T>)
    struct NoThrowMoveConstructibleChecker : public NoThrowConstructibleChecker<T, AddRValueRefType<T>>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowMoveConstructible = NoThrowMoveConstructibleChecker<T>::Value;

    template<typename T>
        requires(IsCompleteOrUnbounded<T>)
    struct NoThrowMoveAssignableChecker : NoThrowAssignableChecker<AddLValueRefType<T>, AddRValueRefType<T>>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowMoveAssignable = NoThrowMoveAssignableChecker<T>::Value;

    template<typename T>
        requires(IsCompleteOrUnbounded<T>)
    struct NoThrowCopyConstructibleChecker : public NoThrowConstructibleChecker<T, AddLValueRefType<const T>>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowCopyConstructible = NoThrowCopyConstructibleChecker<T>::Value;

    template<typename T>
        requires(IsCompleteOrUnbounded<T>)
    struct NoThrowCopyAssignableChecker : NoThrowAssignableChecker<AddLValueRefType<T>, AddLValueRefType<const T>>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowCopyAssignable = NoThrowCopyAssignableChecker<T>::Value;

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowDefaultConstructible = __is_nothrow_constructible(T);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsCopyConstructible = __is_constructible(T, AddLValueRefType<const T>);

    template<typename T, typename... Args>
    SSSENGINE_GLOBAL
    constexpr bool IsConstructible = __is_constructible(T, Args...);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsMoveConstructible = __is_constructible(T, AddRValueRefType<T>);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsDefaultConstructible = __is_constructible(T);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsCopyAssignable = __is_assignable(AddLValueRefType<T>, AddLValueRefType<const T>);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsMoveAssignable = __is_assignable(AddLValueRefType<T>, AddRValueRefType<T>);

    template<typename T, typename U>
    SSSENGINE_GLOBAL
    constexpr bool IsAssignable = __is_assignable(T, U);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsTriviallyDestructible = __is_trivially_destructible(T);

    struct IsNoThrowDestructibleChecker
    {
        template<typename T>
        constexpr static BoolConstant<noexcept(DeclVal<T &>().~T())> Test(int);

        template<typename>
        constexpr static FalseType Test(...);
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowDestructible = __is_nothrow_destructible(T);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsDestructible = __is_destructible(T);

    template<typename T, typename... Args>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowConstructible = __is_nothrow_constructible(T, Args...);

    template<typename From, typename To>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowConvertible = __is_nothrow_convertible(From, To);

    template<typename From, typename To>
    SSSENGINE_GLOBAL
    constexpr bool IsNoThrowAssignable = __is_nothrow_assignable(From, To);

} // namespace SSSEngine
