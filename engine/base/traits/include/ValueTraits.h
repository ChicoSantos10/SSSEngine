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
 * @brief Traits for basic Types
 */

#pragma once

#include "ConversionTraits.h"
#include "EnumTraits.h"
#include "QualifierTraits.h"
#include "Traits.h"
#include "Types.h"
#include "ValueConstant.h"
#include "HelperMacros.h"

namespace SSSEngine
{
    template<typename>
    struct IsIntegerImpl : public FalseType
    {
    };

    template<>
    struct IsIntegerImpl<i8> : public TrueType
    {
    };

    template<>
    struct IsIntegerImpl<u8> : public TrueType
    {
    };

    template<>
    struct IsIntegerImpl<i16> : public TrueType
    {
    };

    template<>
    struct IsIntegerImpl<u16> : public TrueType
    {
    };

    template<>
    struct IsIntegerImpl<i32> : public TrueType
    {
    };

    template<>
    struct IsIntegerImpl<u32> : public TrueType
    {
    };

    template<>
    struct IsIntegerImpl<i64> : public TrueType
    {
    };

    template<>
    struct IsIntegerImpl<u64> : public TrueType
    {
    };

    template<>
    struct IsIntegerImpl<i128> : public TrueType
    {
    };

    template<>
    struct IsIntegerImpl<u128> : public TrueType
    {
    };

    template<typename T>
    struct Integer : public IsIntegerImpl<RemoveCVType<T>>::Type
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsInteger = Integer<T>::Value;

    template<typename T>
    struct IsFloatingPointImpl : public FalseType
    {
    };

    template<>
    struct IsFloatingPointImpl<float> : public TrueType
    {
    };

    template<>
    struct IsFloatingPointImpl<double> : public TrueType
    {
    };

    template<>
    struct IsFloatingPointImpl<long double> : public TrueType
    {
    };

    template<typename T>
    struct FloatingPointChecker : IsFloatingPointImpl<RemoveCVType<T>>::Type
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsFloatingPoint = FloatingPointChecker<T>::Value;

    template<typename T>
    struct NumberChecker : Or<Integer<T>, FloatingPointChecker<T>>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsNumber = IsInteger<T> || IsFloatingPoint<T>;

    template<typename T>
    struct UnicodeTypeCheckerImpl : public FalseType
    {
    };

    template<>
    struct UnicodeTypeCheckerImpl<char8> : public TrueType
    {
    };

    template<>
    struct UnicodeTypeCheckerImpl<char16> : public TrueType
    {
    };

    template<>
    struct UnicodeTypeCheckerImpl<char32> : public TrueType
    {
    };

    template<typename T>
    struct UnicodeTypeChecker : UnicodeTypeCheckerImpl<RemoveCVType<T>>::Type
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsUnicodeChar = UnicodeTypeChecker<T>::Value;

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsASCIIChar = IsSameType<RemoveCVType<T>, char>;

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsWideChar = IsSameType<RemoveCVType<T>, wchar_t>;

    template<typename T>
    struct NullPointerChecker : FalseType
    {
    };

    template<>
    struct NullPointerChecker<NullPtrType> : TrueType
    {
    };

    template<>
    struct NullPointerChecker<const NullPtrType> : TrueType
    {
    };

    template<>
    struct NullPointerChecker<volatile NullPtrType> : TrueType
    {
    };

    template<>
    struct NullPointerChecker<const volatile NullPtrType> : TrueType
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsNullPointer = NullPointerChecker<T>::Value;

    template<typename T>
    struct ScalarChecker :
        Or<NumberChecker<T>, EnumChecker<T>, PointerChecker<T>, MemberPointerChecker<T>, NullPointerChecker<T>>
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsScalar = ScalarChecker<T>::Value;
} // namespace SSSEngine
