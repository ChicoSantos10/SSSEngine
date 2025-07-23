/*  SSS Engine
    Copyright (C) 2024  Francisco Santos

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <type_traits>
#include <concepts>

// TODO: Rewrite the std library functions
namespace SSSEngine
{
    template<typename T>
    concept NumberConcept = std::is_arithmetic_v<T>;

    template<typename T>
    concept IntegralConcept = std::integral<T>;

    template<typename T>
    concept RealConcept = std::floating_point<T>;

    template<typename T, typename... Others>
    concept EqualTypesConcept = std::conjunction_v<std::is_same<T, Others>...>;

    template<typename T>
    concept ArithmeticConcept = requires(T t) {
        { t + t } -> std::convertible_to<T>;
        { t - t } -> std::convertible_to<T>;
        { t *t } -> std::convertible_to<T>;
        { t / t } -> std::convertible_to<T>;
        { t = t };
        { t += t };
        { t -= t };
    };

    template<typename T>
    concept LogicalConcept = requires(T t) {
        { t &&t } -> std::convertible_to<bool>;
        { t || t } -> std::convertible_to<bool>;
    };

    template<typename T>
    concept ArithmeticComparableConcept = ArithmeticConcept<T> && LogicalConcept<T>;

    template<typename T>
    concept FunctionPointerConcept = std::is_pointer_v<T> && std::is_function_v<typename std::remove_pointer_t<T>>;

    template<typename T, template<typename...> typename V>
    inline constexpr bool IsInstanceOfV = std::false_type{};

    template<template<typename...> typename V, typename... Vs>
    inline constexpr bool IsInstanceOfV<V<Vs...>, V> = std::true_type{};

    template<typename T, typename V>
    inline constexpr bool IsLikeV = std::is_same_v<T, V>;

    struct _
    {
    };

    template<typename T>
    inline constexpr bool IsLikeV<T, _> = true;

    template<template<typename...> typename T, typename... Ts, typename... Us>
        requires(sizeof...(Ts) == sizeof...(Us))
    inline constexpr bool IsLikeV<T<Ts...>, T<Us...>> = (IsLikeV<Ts, Us> && ...);
} // namespace SSSEngine
