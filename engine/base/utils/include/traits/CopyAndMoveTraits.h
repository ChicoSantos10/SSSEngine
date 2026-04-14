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

#include "HelperMacros.h"
#include "QualifierTraits.h"
#include "ValueConstant.h"

namespace SSSEngine
{

    template<typename T>
        requires(IsCompleteOrUnbounded<T>)
    struct TriviallyCopyableChecker : public BoolConstant<__is_trivially_copyable(T)>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsTriviallyCopyable = TriviallyCopyableChecker<T>::Value;

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsBitwiseCopyable = __is_bitwise_cloneable(T);

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
    SSSENGINE_GLOBAL constexpr bool IsNoThrowMoveConstructible = NoThrowMoveConstructibleChecker<T>::Value;

    template<typename T>
        requires(IsCompleteOrUnbounded<T>)
    struct NoThrowMoveAssignableChecker : NoThrowAssignableChecker<AddLValueRefType<T>, AddRValueRefType<T>>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsNoThrowMoveAssignable = NoThrowMoveAssignableChecker<T>::Value;

    template<typename T>
        requires(IsCompleteOrUnbounded<T>)
    struct NoThrowCopyConstructibleChecker : public NoThrowConstructibleChecker<T, AddLValueRefType<const T>>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsNoThrowCopyConstructible = NoThrowCopyConstructibleChecker<T>::Value;

    template<typename T>
        requires(IsCompleteOrUnbounded<T>)
    struct NoThrowCopyAssignableChecker : NoThrowAssignableChecker<AddLValueRefType<T>, AddLValueRefType<const T>>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsNoThrowCopyAssignable = NoThrowCopyAssignableChecker<T>::Value;

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsNoThrowDefaultConstructible = __is_nothrow_constructible(T);

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsCopyConstructible = __is_constructible(T, AddLValueRefType<const T>);

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsMoveConstructible = __is_constructible(T, AddRValueRefType<T>);

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsDefaultConstructible = __is_constructible(T);

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsCopyAssignable = __is_assignable(AddLValueRefType<T>, AddLValueRefType<const T>);

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsMoveAssignable = __is_assignable(AddLValueRefType<T>, AddRValueRefType<T>);

} // namespace SSSEngine
