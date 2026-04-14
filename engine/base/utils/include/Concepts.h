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
 * @brief Multiple concepts
 */

#pragma once

#include "ConversionTraits.h"
#include "EnumTraits.h"
#include "QualifierTraits.h"
#include "SignTraits.h"
#include "ValueTraits.h"

namespace SSSEngine
{
    template<typename From, typename To>
    concept ConvertibleToConcept = IsConvertible<From, To>;

    template<typename T, typename U>
    concept LessThanComparableConcept = requires(const T &x, const U &y) {
        { x < y } -> ConvertibleToConcept<bool>;
    };

    template<typename T, typename U>
    concept GreaterThanComparableConcept = requires(const T &x, const U &y) {
        { x > y } -> ConvertibleToConcept<bool>;
    };

    /**
     * @brief Concept of a Integral of Floating point number
     *
     */
    template<typename T>
    concept NumberConcept = IsNumber<T>;

    /**
     * @brief Concept of an Integer type
     *
     */
    template<typename T>
    concept IntegralConcept = NumberConcept<T> && IsIntegral<T>;

    /**
     * @brief Concept of a Unsigned Integer type
     *
     */
    template<typename T>
    concept UnsignedIntegralConcept = IntegralConcept<T> && IsUnsigned<T>;

    /**
     * @brief Concept of a Signed Integer type
     *
     */
    template<typename T>
    concept SignedIntegralConcept = IntegralConcept<T> && IsSigned<T>;

    /**
     * @brief Concept of a Floating point Real Number type
     *
     * @tparam T A type representing a floating type number
     */
    template<typename T>
    concept RealConcept = NumberConcept<T> && IsReal<T>;

    /**
     * @brief Compares 2 types to see if they are the same
     *
     */
    template<typename T, typename U>
    concept SameAsConcept = IsSameType<T, U>;

    /**
     * @brief Checks if T and others are of the same type
     *
     */
    template<typename T, typename... Others>
    concept AllEqualTypesConcept = (SameAsConcept<T, Others> && ...);

    /**
     * @brief Checks if T and a type from others are equal
     *
     */
    template<typename T, typename... Others>
    concept AnyTypeEqualConcept = (IsAnyType<T, Others...>);

    /**
     * @brief Concept for function pointer
     *
     */
    template<typename T>
    concept FunctionPointerConcept = IsPointer<T> && IsFunction<RemovePointerType<T>>;

    /**
     * @brief Concept for a pointer to a variable
     *
     */
    template<typename T>
    concept PointerConcept = IsPointer<T> && !IsFunction<RemovePointerType<T>>;

    template<typename T>
    concept EnumConcept = IsEnum<T>;

    template<typename T>
    concept UnicodeTypeConcept = IsUnicodeChar<T>;

    template<typename T>
    concept CharTypeConcept = IsChar<T>;

    template<typename T>
    concept StringTypeConcept = IsChar<T> || IsUnicodeChar<T>;

    template<typename T>
    concept ConstConcept = IsConst<RemoveReferenceType<T>>;

    template<typename T>
    concept NotConstConcept = !IsConst<RemoveReferenceType<T>>;

} // namespace SSSEngine
