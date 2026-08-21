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

#include "ConversionTraits.h"
#include "HelperMacros.h"
#include "QualifierTraits.h"
#include "Types.h"
#include "ValueConstant.h"

namespace SSSEngine
{
    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsCStyleArray = false;

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsCStyleArray<T[]> = true;

    template<typename T, SizeType N>
    SSSENGINE_GLOBAL
    constexpr bool IsCStyleArray<T[N]> = true;

    /**
     * @brief Get the number of dimensions of the array
     *
     */
    template<typename Array>
    SSSENGINE_GLOBAL
    constexpr SizeType Dimensions = __array_rank(Array);

    /**
     * @brief Base case. Not array or index out of range
     */
    template<typename T, unsigned Dim = 0>
    SSSENGINE_GLOBAL
    constexpr SizeType CountOf = 0;

    /**
     * @brief Return the count for dimension 0
     *
     */
    template<typename T, SizeType N>
    SSSENGINE_GLOBAL
    constexpr SizeType CountOf<T[N], 0> = N;

    /**
     * @brief Go one dimension lower until we find the count
     *
     */
    template<typename T, unsigned Dim, SizeType N>
    SSSENGINE_GLOBAL
    constexpr SizeType CountOf<T[N], Dim> = CountOf<T, Dim - 1>;

    /**
     * @brief Unknown size array
     *
     */
    template<typename T>
    SSSENGINE_GLOBAL
    constexpr SizeType CountOf<T[], 0> = 0;

    /**
     * @brief Unknown size array in this dimension. Recursing into previous dimensions
     *
     */
    template<typename T, unsigned Dim>
    SSSENGINE_GLOBAL
    constexpr SizeType CountOf<T[], Dim> = CountOf<T, Dim - 1>;

    template<typename T>
    struct RemoveExtent
    {
        using Type = T;
    };

    template<typename T, SizeType S>
    struct RemoveExtent<T[S]>
    {
        using Type = T;
    };

    template<typename T>
    struct RemoveExtent<T[]>
    {
        using Type = T;
    };

    template<typename T>
    using RemoveExtentType = RemoveExtent<T>::Type;

    template<typename>
    struct ArrayKnownBoundsChecker : public FalseType
    {
    };

    template<typename T, SizeType Size>
    struct ArrayKnownBoundsChecker<T[Size]> : public TrueType
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsArrayKnownBounds = ArrayKnownBoundsChecker<T>::Value;

    template<typename T>
    struct ArrayUnknownBoundsChecker : public FalseType
    {
    };

    template<typename T>
    struct ArrayUnknownBoundsChecker<T[]> : public TrueType
    {
    };

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsArrayUnknownBounds = ArrayUnknownBoundsChecker<T>::Value;

    template<typename T, SizeType = sizeof(T)>
    constexpr TrueType CompleteOrUnboundedChecker(Identity<T>)
    {
        return {};
    }

    template<typename TypeIdentity, typename NestedType = typename TypeIdentity::Type>
    constexpr OrType<ReferenceChecker<NestedType>, FunctionChecker<NestedType>, VoidChecker<NestedType>, ArrayUnknownBoundsChecker<NestedType>>
    CompleteOrUnboundedChecker(TypeIdentity)
    {
        return {};
    }

    template<typename T>
    SSSENGINE_GLOBAL
    constexpr bool IsCompleteOrUnbounded = decltype(CompleteOrUnboundedChecker(Identity<T>{}))::Value;

    template<typename From, typename To>
    SSSENGINE_GLOBAL
    constexpr bool IsArrayConvertible = IsConvertible<From (*)[], To (*)[]>;

} // namespace SSSEngine
