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

#include "ArrayTraits.h"
#include "Attributes.h"
#include "ConversionTraits.h"
#include "CopyAndMoveTraits.h"
#include "EnumTraits.h"
#include "ObjectTraits.h"
#include "QualifierTraits.h"
#include "SignTraits.h"
#include "ValueTraits.h"

namespace SSSEngine
{
    template<typename From, typename To>
    concept ConvertibleToConcept = IsConvertible<From, To>;

    // TODO: Use Forward instead of static_cast. For that we can't have utility.h depend on concepts.
    template<typename From, typename To>
    concept NonNarrowingConvertibleToConcept = requires(To &&to) { From{static_cast<To &&>(to)}; };

    template<typename T>
    concept BooleanTestableConcept = ConvertibleToConcept<T, bool> && requires(T &&t) {
        { !static_cast<T &&>(t) } -> ConvertibleToConcept<bool>;
    };

    template<typename T, typename U>
    concept ComparableWithConcept = requires(const RemoveReferenceType<T> &x, const RemoveReferenceType<U> &y) {
        { x < y } -> BooleanTestableConcept;
        { x <= y } -> BooleanTestableConcept;
        { x > y } -> BooleanTestableConcept;
        { x >= y } -> BooleanTestableConcept;
        { y < x } -> BooleanTestableConcept;
        { y <= x } -> BooleanTestableConcept;
        { y > x } -> BooleanTestableConcept;
        { y >= x } -> BooleanTestableConcept;
    };

    template<typename T>
    concept ComparableConcept = ComparableWithConcept<T, T>;

    template<typename T, typename U>
    concept EqualityComparableWithConcept = requires(const RemoveReferenceType<T> &t, const RemoveReferenceType<U> &u) {
        { t == u } -> BooleanTestableConcept;
        { t != u } -> BooleanTestableConcept;
        { u == t } -> BooleanTestableConcept;
        { u != t } -> BooleanTestableConcept;
    };

    template<typename T>
    concept EqualityComparableConcept = EqualityComparableWithConcept<T, T>;

    template<typename T, typename U>
    concept TotallyComparableWithConcept = EqualityComparableWithConcept<T, U> && ComparableWithConcept<T, U>;

    template<typename T>
    concept TotallyComparableConcept = TotallyComparableWithConcept<T, T>;

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

    template<typename T>
    concept ObjectConcept = IsObject<T>;

    template<typename T>
    concept ArrayConcept = IsArray<T>;

    template<typename Tp, typename Up>
    concept CommonReferenceWithConcept =
        SameAsConcept<CommonReferenceType<Tp, Up>, CommonReferenceType<Up, Tp>> &&
        ConvertibleToConcept<Tp, CommonReferenceType<Tp, Up>> && ConvertibleToConcept<Up, CommonReferenceType<Tp, Up>>;

    template<typename T, typename U>
    concept AssignableFromConcept =
        IsLValueReference<T> && CommonReferenceWithConcept<RemoveReferenceType<T> &, RemoveReferenceType<U> &> &&
        requires(T t, U u) {
            { t = static_cast<U &&>(u) } -> SameAsConcept<T>;
        };

    template<typename T>
    concept CopyConstructibleConcept = IsCopyConstructible<T>;

    template<typename T>
    concept MoveConstructibleConcept = IsMoveConstructible<T>;

    template<typename T>
    concept ReferenceableConcept = requires { typename Identity<T &>; };

    template<typename T>
    concept DereferenceableConcept = requires(T &t) {
        { *t } -> ReferenceableConcept;
    };

    namespace SSSENGINE_HIDDEN Impl
    {
        template<typename T>
        constexpr bool DestructibleImpl = false;
        template<typename T>
            requires requires(T &t) {
                { t.~T() } noexcept;
            }
        constexpr bool DestructibleImpl<T> = true;

        template<typename T>
        constexpr bool Destructible = DestructibleImpl<T>;
        template<typename T>
        constexpr bool Destructible<T &> = true;
        template<typename T>
        constexpr bool Destructible<T &&> = true;
        template<typename T, SizeType N>
        constexpr bool Destructible<T[N]> = DestructibleImpl<T>;
    } // namespace SSSENGINE_HIDDEN Impl

    template<typename T>
    concept DestructibleConcept = Impl::Destructible<T>;

    template<typename T, typename... Args>
    concept ConstructibleFromConcept = DestructibleConcept<T> && IsConstructible<T, Args...>;

    template<typename T>
    concept DefaultInitializableConcept = ConstructibleFromConcept<T> && requires {
        T{};
        (void)::new T;
    };

    template<typename T>
    concept ClassOrEnumConcept = IsClass<T> || IsUnion<T> || IsEnum<T>;

    template<typename T>
    concept HasValueTypeConcept = requires { typename T::ValueType; };

    template<typename T>
    concept HasElementTypeConcept = requires { typename T::ElementType; };

} // namespace SSSEngine
