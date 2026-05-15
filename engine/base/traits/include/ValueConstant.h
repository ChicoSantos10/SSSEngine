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
 * @brief File with structs used for determing false, true of other constant types
 */

#pragma once

#include "Types.h"

namespace SSSEngine
{
    template<typename T, T V>
    struct ValueConstant
    {
        static constexpr T Value = V;
        using ValueType = T;
        using Type = ValueConstant<T, V>;

        constexpr operator ValueType() const noexcept // NOLINT(*-explicit-constructor)
        {
            return Value;
        }

        constexpr ValueType operator()() const noexcept
        {
            return Value;
        }
    };

    template<bool V>
    using BoolConstant = ValueConstant<bool, V>;
    using TrueType = BoolConstant<true>;
    using FalseType = BoolConstant<false>;

    template<SizeType N>
    using IntegralConstant = ValueConstant<SizeType, N>;
} // namespace SSSEngine
