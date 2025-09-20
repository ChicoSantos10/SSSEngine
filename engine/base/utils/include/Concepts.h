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

#include "HelperMacros.h"
#include <type_traits>
#include <concepts>

// TODO: Rewrite the std library functions
namespace SSSEngine
{
    template<typename T, typename U>
    concept LessThanComparableConcept = requires(const T &x, const U &y) {
        { x < y } -> std::convertible_to<bool>;
    };

    template<typename T, typename U>
    concept GreaterThanComparableConcept = requires(const T &x, const U &y) {
        { x > y } -> std::convertible_to<bool>;
    };

    /**
     * @brief Concept of a Integral of Floating point number
     *
     */
    template<typename T>
    concept NumberConcept = std::is_arithmetic_v<T>;

    /**
     * @brief Concept of an Integer type
     *
     */
    template<typename T>
    concept IntegralConcept = NumberConcept<T> && std::integral<T>; // TODO: Bools should not be considered integral
                                                                    // types

    /**
     * @brief Concept of a Floating point Real Number type
     *
     * @tparam T A type representing a floating type number
     */
    template<typename T>
    concept RealConcept = NumberConcept<T> && std::floating_point<T>;

    /**
     * @brief Compares 2 types to see if they are the same
     *
     */
    template<typename T, typename U>
    concept SameAsConcept = std::same_as<T, U>;

    /**
     * @brief Checks if T and others are of the same type
     *
     */
    template<typename T, typename... Others>
    concept AllEqualTypesConcept = (SameAsConcept<T, Others> && ...);

    /**
     * @brief Concept for function pointer
     *
     */
    template<typename T>
    concept FunctionPointerConcept = std::is_pointer_v<T> && std::is_function_v<typename std::remove_pointer_t<T>>;

    /**
     * @brief Checks if T is instance of V
     *
     */
    template<typename T, template<typename...> typename V>
    inline constexpr bool IsInstanceOfV = std::false_type{};

    /**
     * @brief Checks if T is instance of V
     *
     */
    template<template<typename...> typename V, typename... Vs>
    inline constexpr bool IsInstanceOfV<V<Vs...>, V> = std::true_type{};

    /**
     * @brief Checks if T and V are the same
     *
     */
    template<typename T, typename V>
    inline constexpr bool IsLikeV = std::is_same_v<T, V>;

    struct _
    {
    };

    /**
     * @brief _ is a universal type
     *
     */
    template<typename T>
    inline constexpr bool IsLikeV<T, _> = true;

    /**
     * @brief Checks if type T with template parameters Ts is the same as T with parameters Us
     *
     */
    template<template<typename...> typename T, typename... Ts, typename... Us>
        requires(sizeof...(Ts) == sizeof...(Us))
    inline constexpr bool IsLikeV<T<Ts...>, T<Us...>> = (IsLikeV<Ts, Us> && ...);

    template<typename T>
    concept EnumConcept = std::is_enum_v<T>;

    // TODO: Below this line are traits, not concepts. Move them to a traits file
    // Also above there are others that should move
    template<EnumConcept T>
    using UnderlyingType = std::underlying_type_t<T>;

    template<typename T>
    using UnsignedType = std::make_unsigned_t<T>;

    template<typename T, typename U>
    SSSENGINE_GLOBAL constexpr bool SameType = std::is_same_v<T, U>;

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsUnsigned = SameType<T, UnsignedType<T>>;

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool IsSigned = std::is_signed_v<T>;

    template<typename... Args>
    using CommonType = std::common_type_t<Args...>;

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool TriviallyCopyable = std::is_trivially_copyable_v<T>;

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool NoThrowMoveConstructible = std::is_nothrow_move_constructible_v<T>;

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool NoThrowMoveAssignable = std::is_nothrow_move_assignable_v<T>;

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool NoThrowCopyConstructible = std::is_nothrow_copy_constructible_v<T>;

    template<typename T>
    SSSENGINE_GLOBAL constexpr bool NoThrowCopyAssignable = std::is_nothrow_copy_assignable_v<T>;
} // namespace SSSEngine
