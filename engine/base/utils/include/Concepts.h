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

#include <type_traits>
#include <concepts>

// TODO: Rewrite the std library functions
namespace SSSEngine
{
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
    concept IntegralConcept = std::integral<T>;

    /**
     * @brief Concept of a Floating point Real Number type
     *
     * @tparam T [TODO:tparam]
     */
    template<typename T>
    concept RealConcept = std::floating_point<T>;

    /**
     * @brief Checks if T and others are of the same type
     *
     */
    template<typename T, typename... Others>
    concept EqualTypesConcept = std::conjunction_v<std::is_same<T, Others>...>;

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
     * @brief Checks is T and V are the same
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
} // namespace SSSEngine
