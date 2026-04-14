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
#include "QualifierTraits.h"
#include "ValueConstant.h"
#include "HelperMacros.h"

namespace SSSEngine
{
    template<typename>
    struct IsIntegralImpl : public FalseType
    {
    };

    template<>
    struct IsIntegralImpl<bool> : public FalseType
    {
    };

    template<>
    struct IsIntegralImpl<char> : public TrueType
    {
    };

    template<>
    struct IsIntegralImpl<signed char> : public TrueType
    {
    };

    template<>
    struct IsIntegralImpl<unsigned char> : public TrueType
    {
    };

    template<>
    struct IsIntegralImpl<wchar_t> : public TrueType
    {
    };

    template<>
    struct IsIntegralImpl<short> : public TrueType
    {
    };

    template<>
    struct IsIntegralImpl<unsigned short> : public TrueType
    {
    };

    template<>
    struct IsIntegralImpl<int> : public TrueType
    {
    };

    template<>
    struct IsIntegralImpl<unsigned int> : public TrueType
    {
    };

    template<>
    struct IsIntegralImpl<long> : public TrueType
    {
    };

    template<>
    struct IsIntegralImpl<unsigned long> : public TrueType
    {
    };

    template<>
    struct IsIntegralImpl<long long> : public TrueType
    {
    };

    template<>
    struct IsIntegralImpl<unsigned long long> : public TrueType
    {
    };

    template<typename T>
    struct Integral : public IsIntegralImpl<RemoveCVType<T>>::Type
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsIntegral = Integral<T>::Value;

    template<typename T>
    struct IsRealImpl : public FalseType
    {
    };

    template<>
    struct IsRealImpl<float> : public TrueType
    {
    };

    template<>
    struct IsRealImpl<double> : public TrueType
    {
    };

    template<>
    struct IsRealImpl<long double> : public TrueType
    {
    };

    template<typename T>
    struct Real : IsRealImpl<RemoveCVType<T>>::Type
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsReal = Real<T>::Value;

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsNumber = IsIntegral<T> || IsReal<T>;

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
    SSSENGINE_GLOBAL constexpr bool IsUnicodeChar = UnicodeTypeChecker<T>::Value;

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsChar = IsAnyType<RemoveCVType<T>, char, unsigned char, wchar_t>;

} // namespace SSSEngine
