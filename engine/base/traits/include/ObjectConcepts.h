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
 * @brief
 */

#pragma once

#include "Concepts.h"
#include "ObjectTraits.h"
#include "Swap.h"

namespace SSSEngine
{

    template<typename T>
    concept MovableConcept =
        IsObject<T> && MoveConstructibleConcept<T> && AssignableFromConcept<T &, T> && SwappableConcept<T>;

    template<typename T>
    concept CopyableConcept = CopyConstructibleConcept<T> && MovableConcept<T> && AssignableFromConcept<T &, T &> &&
                              AssignableFromConcept<T &, const T &> && AssignableFromConcept<T &, const T>;

    template<typename T>
    concept SemiregularConcept = CopyableConcept<T> && DefaultInitializableConcept<T>;

    template<typename T>
    concept RegularConcept = SemiregularConcept<T> && EqualityComparableWithConcept<T, T>;
} // namespace SSSEngine
