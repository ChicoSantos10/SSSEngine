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
 * @brief Traits for dealing if conversions between Types
 */

#pragma once

#include "HelperMacros.h"
#include "QualifierTraits.h"
#include "ValueConstant.h"

namespace SSSEngine
{
    template<typename From, typename To>
    struct ConvertibleChecker : BoolConstant<__is_convertible(From, To)>
    {
    };

    template<typename From, typename To>
    SSSENGINE_GLOBAL constexpr bool IsConvertible = ConvertibleChecker<From, To>::Value;

    template<typename... T>
    struct CommonTypeChecker;

    struct CommonTypeImpl
    {
        template<typename T, typename V>
        using DeducedType = decltype(true ? DeclVal<T>() : DeclVal<V>());

        template<typename T, typename V>
        static SuccessType<DecayType<DeducedType<T, V>>> Test(int);

        template<typename T, typename V>
        static SuccessType<RemoveCVReferenceType<DeducedType<T &, V &>>> Test2(int);

        template<typename T, typename V>
        static FailureType Test2(...);

        template<typename T, typename V>
        static decltype(Test2<T, V>(0)) Test(...);
    };

    template<>
    struct CommonTypeChecker<>
    {
    };

    template<typename T>
    struct CommonTypeChecker<T> : public CommonTypeChecker<T, T>
    {
    };

    template<typename T0, typename T1, typename D0 = DecayType<T0>, typename D1 = DecayType<T1>>
    struct CommonTypeHelper
    {
        using Type = CommonTypeChecker<D0, D1>;
    };

    template<typename T0, typename T1>
    struct CommonTypeHelper<T0, T1, T0, T1> : private CommonTypeImpl
    {
        using Type = decltype(Test<T0, T1>(0));
    };

    template<typename T0, typename T1>
    struct CommonTypeChecker<T0, T1> : public CommonTypeHelper<T0, T1>::Type
    {
    };

    template<typename...>
    struct CommonTypePack
    {
    };

    template<typename, typename, typename = void>
    struct CommonTypeFold;

    template<typename T0, typename T1, typename... Rest>
    struct CommonTypeChecker<T0, T1, Rest...> : public CommonTypeFold<CommonTypeChecker<T0, T1>, CommonTypePack<Rest...>>
    {
    };

    template<typename T0, typename... Args>
    struct CommonTypeFold<T0, CommonTypePack<Args...>, VoidType<typename T0::Type>> :
    public CommonTypeChecker<typename T0::Type, Args...>
    {
    };

    template<typename T0, typename T1>
    struct CommonTypeFold<T0, T1, void>
    {
    };

    template<typename... Args>
    using CommonType = typename CommonTypeChecker<Args...>::Type;

    template<typename From, typename To>
    SSSENGINE_GLOBAL constexpr bool IsSameType = __is_same(From, To);

    template<typename T, typename... Args>
    SSSENGINE_GLOBAL constexpr bool IsAnyType = (IsSameType<T, Args> || ...);

} // namespace SSSEngine
