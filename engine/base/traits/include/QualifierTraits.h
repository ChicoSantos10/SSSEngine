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
 * @brief Traits for removing or adding qualifiers to Types
 */

#pragma once

#include "HelperMacros.h"
#include "Traits.h"
#include "Types.h"
#include "ValueConstant.h"

namespace SSSEngine
{
    template<typename>
    struct ConstChecker : FalseType
    {
    };

    template<typename T>
    struct ConstChecker<T const> : TrueType
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsConst = ConstChecker<T>::Value;

    template<typename>
    struct VolatileChecker : FalseType
    {
    };

    template<typename T>
    struct VolatileChecker<T volatile> : TrueType
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsVolatile = VolatileChecker<T>::Value;

    template<typename T>
    struct RemoveCV
    {
        using Type = __remove_cv(T);
    };

    template<typename T>
    using RemoveCVType = typename RemoveCV<T>::Type;

    template<typename T>
    struct RemoveVolatile
    {
        using Type = T;
    };

    template<typename T>
    struct RemoveVolatile<T volatile>
    {
        using Type = T;
    };

    template<typename T>
    using RemoveVolatileType = typename RemoveVolatile<T>::Type;

    template<typename T, bool IsConst, bool IsVolatile>
    struct CVSelector;

    template<typename T>
    struct CVSelector<T, false, false>
    {
        using Type = T;
    };

    template<typename T>
    struct CVSelector<T, true, false>
    {
        using Type = const T;
    };

    template<typename T>
    struct CVSelector<T, false, true>
    {
        using Type = volatile T;
    };

    template<typename T>
    struct CVSelector<T, true, true>
    {
        using Type = const volatile T;
    };

    template<typename Qualified, typename Unqualified>
    class MatchCV

    {
        using Match = CVSelector<Unqualified, IsConst<Qualified>, IsVolatile<Qualified>>;

      public:
        using Type = typename Match::Type;
    };

    template<typename T>
    using DecayType = __decay(T);

    template<typename T>
    struct RemoveReference
    {
        using Type = T;
    };

    template<typename T>
    struct RemoveReference<T &>
    {
        using Type = T;
    };

    template<typename T>
    struct RemoveReference<T &&>
    {
        using Type = T;
    };

    template<typename T>
    struct RemovePointer
    {
        using Type = __remove_pointer(T);
    };

    template<typename T>
    using RemoveReferenceType = typename RemoveReference<T>::Type;

    template<typename T>
    using RemoveCVReferenceType = RemoveCVType<RemoveReferenceType<T>>;

    template<typename T>
    using RemovePointerType = typename RemovePointer<T>::Type;

    template<typename T>
    struct ReferenceChecker : public BoolConstant<__is_reference(T)>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsReference = ReferenceChecker<T>::Value;

    template<typename T>
    struct FunctionChecker : public BoolConstant<__is_function(T)>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsFunction = FunctionChecker<T>::Value;

    template<typename T>
    struct PointerChecker : BoolConstant<__is_pointer(T)>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsPointer = PointerChecker<T>::Value;

    template<typename>
    struct VoidChecker : public FalseType
    {
    };

    template<>
    struct VoidChecker<void> : public TrueType
    {
    };

    template<>
    struct VoidChecker<const void> : public TrueType
    {
    };

    template<>
    struct VoidChecker<volatile void> : public TrueType
    {
    };

    template<>
    struct VoidChecker<const volatile void> : public TrueType
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsVoid = VoidChecker<T>::Value;

    template<typename T>
    struct Identity
    {
        using Type = T;
    };

    template<typename T>
    using IdentityType = Identity<T>::Type;

    template<typename T>
    using AddLValueRefType = __add_lvalue_reference(T);

    template<typename T>
    using AddRValueRefType = __add_rvalue_reference(T);

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsLValueReference = false;

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsLValueReference<T &> = true;

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsRValueReference = false;

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsRValueReference<T &&> = true;

    template<typename From, typename To>
    using MatchCVQualifiers = CVSelector<To, IsConst<From>, IsVolatile<From>>;

    template<typename From, typename To>
    using MatchCVQualifiersType = MatchCVQualifiers<From, To>::Type;

    template<typename T, typename = void>
    struct AddPointerHelper
    {
        using Type = T;
    };

    template<typename T>
    struct AddPointerHelper<T, VoidType<T *>>
    {
        using Type = T *;
    };

    template<typename T>
    struct AddPointer : public AddPointerHelper<T>
    {
    };

    template<typename T>
    struct AddPointer<T &>
    {
        using Type = T *;
    };

    template<typename T>
    struct AddPointer<T &&>
    {
        using Type = T *;
    };

    template<typename T>
    using AddPointerType = AddPointer<T>::Type;

} // namespace SSSEngine
