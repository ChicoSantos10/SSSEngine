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

#include <concepts>

namespace SSSEngine
{
    template <typename T> concept Number = std::is_arithmetic_v<T>;
    template <typename T> concept Integral = std::integral<T>;
    template <typename T> concept Real = std::floating_point<T>;

    template <typename T>concept Arithmetic = requires(T t)
    {
        { t + t } -> std::convertible_to<T>;
        { t - t } -> std::convertible_to<T>;
        { t * t } -> std::convertible_to<T>;
        { t / t } -> std::convertible_to<T>;
        { t = t };
        { t += t };
        { t -= t };
    };

    template <typename T>concept Logical = requires(T t)
    {
        { t && t } -> std::convertible_to<bool>;
        { t || t } -> std::convertible_to<bool>;
    };

    template <typename T>concept ArithmeticComparable = Arithmetic<T> && Logical<T>;

    template <typename T, typename... Args>concept Function = requires(T t, Args... args)
        {
            t(args);
        }
        || requires(T t)
        {
            t();
        };
}
