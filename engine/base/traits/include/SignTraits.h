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
 * @brief Traits for determining or swapping the sign of a Type
 */

#pragma once

#include "Attributes.h"
#include "ConversionTraits.h"
#include "HelperMacros.h"
#include "EnumTraits.h"
#include "QualifierTraits.h"
#include "ValueTraits.h"

namespace SSSEngine
{
    template<typename T>
    struct UnsignedImpl
    {
        using Type = T;
    };

    template<>
    struct UnsignedImpl<char>
    {
        using Type = unsigned char;
    };

    template<>
    struct UnsignedImpl<signed char>
    {
        using Type = unsigned char;
    };

    template<>
    struct UnsignedImpl<short>
    {
        using Type = unsigned short;
    };

    template<>
    struct UnsignedImpl<int>
    {
        using Type = unsigned int;
    };

    template<>
    struct UnsignedImpl<long>
    {
        using Type = unsigned long;
    };

    template<>
    struct UnsignedImpl<long long>
    {
        using Type = unsigned long long;
    };

    template<typename T>
    class UnsignedSelector;

    template<typename T>
        requires IsIntegral<T>
    class UnsignedSelector<T>
    {
        using UnsignedType = typename UnsignedImpl<RemoveCVType<T>>::Type;

      public:
        using Type = typename MatchCV<T, UnsignedType>::Type;
    };

    template<typename T>
        requires IsEnum<T>
    class UnsignedSelector<T>
    {
        using Underlying = UnderlyingType<T>;
        using UnsignedType = typename UnsignedImpl<RemoveCVType<Underlying>>::Type;

      public:
        using Type = typename MatchCV<Underlying, UnsignedType>::Type;
    };

    template<typename T>
    struct MakeUnsigned
    {
        using Type = typename UnsignedSelector<T>::Type;
    };

    /**
     * @brief Gets the unsigned version of Type T. If it's already unsigned just returns itself
     *
     * @example
     * unsigned int -> unsigned int
     * unsigned long -> unsigned long
     * int -> unsigned int
     * long -> unsigned long
     *
     * @tparam T The Type to get the signed version
     */
    template<typename T>
    using UnsignedType = MakeUnsigned<T>::Type;

    template<typename T>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr auto ToUnsigned(T t)
    {
        return static_cast<UnsignedType<T>>(t);
    }

    template<typename T>
    struct SignedImpl
    {
        using Type = T;
    };

    template<>
    struct SignedImpl<char>
    {
        using Type = signed char;
    };

    template<>
    struct SignedImpl<unsigned char>
    {
        using Type = signed char;
    };

    template<>
    struct SignedImpl<unsigned short>
    {
        using Type = short;
    };

    template<>
    struct SignedImpl<unsigned int>
    {
        using Type = int;
    };

    template<>
    struct SignedImpl<unsigned long>
    {
        using Type = long;
    };

    template<>
    struct SignedImpl<unsigned long long>
    {
        using Type = long long;
    };

    template<typename T>
    class SignedSelector;

    template<typename T>
        requires IsIntegral<T>
    class SignedSelector<T>
    {
        using SignedType = typename SignedImpl<RemoveCVType<T>>::Type;

      public:
        using Type = typename MatchCV<T, SignedType>::Type;
    };

    template<typename T>
        requires IsEnum<T>
    class SignedSelector<T>
    {
        using Underlying = UnderlyingType<T>;
        using SignedType = typename SignedImpl<RemoveCVType<Underlying>>::Type;

      public:
        using Type = typename MatchCV<Underlying, SignedType>::Type;
    };

    template<typename T>
    struct MakeSigned
    {
        using Type = typename SignedSelector<T>::Type;
    };

    /**
     * @brief Gets the signed version of Type T. If it's already signed just returns itself
     *
     * @example
     * unsigned int -> int
     * unsigned long -> long
     * int -> int
     * long -> long
     *
     * @tparam T The Type to get the signed version
     */
    template<typename T>
    using SignedType = MakeSigned<T>::Type;

    template<typename T>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    auto ToSigned(T t)
    {
        return static_cast<SignedType<T>>(t);
    }

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsUnsigned = IsSameType<T, UnsignedType<T>>;

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsSigned = IsSameType<T, SignedType<T>>;
} // namespace SSSEngine
