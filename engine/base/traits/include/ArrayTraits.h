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
 * @brief Raw Arrays traits utilities
 */

#pragma once

#include "HelperMacros.h"
#include "Types.h"

namespace SSSEngine::Utils
{
    /**
     * @brief Get the number of dimensions of the array
     *
     */
    template<typename Array>
    SSSENGINE_GLOBAL constexpr Size Dimensions = __array_rank(Array);

    /**
     * @brief Base case. Not array or index out of range
     */
    template<typename T, unsigned Dim = 0>
    SSSENGINE_GLOBAL constexpr Size CountOf = 0;

    /**
     * @brief Return the count for dimension 0
     *
     */
    template<typename T, Size N>
    SSSENGINE_GLOBAL constexpr Size CountOf<T[N], 0> = N;

    /**
     * @brief Go one dimension lower until we find the count
     *
     */
    template<typename T, unsigned Dim, Size N>
    SSSENGINE_GLOBAL constexpr Size CountOf<T[N], Dim> = CountOf<T, Dim - 1>;

    /**
     * @brief Unknown size array
     *
     */
    template<typename T>
    SSSENGINE_GLOBAL constexpr Size CountOf<T[], 0> = 0;

    /**
     * @brief Unknown size array in this dimension. Recursing into previous dimensions
     *
     */
    template<typename T, unsigned Dim>
    SSSENGINE_GLOBAL constexpr Size CountOf<T[], Dim> = CountOf<T, Dim - 1>;

} // namespace SSSEngine::Utils
