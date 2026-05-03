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
 * @brief File for utility functions
 */

#pragma once

#include "Attributes.h"
#include "Concepts.h"
#include "ConversionTraits.h"
#include "CopyAndMoveTraits.h"
#include "HelperMacros.h"
#include "QualifierTraits.h"

// TODO: Remove std library and implement placement new functions
// SSSENGINE_FORCE_INLINE
// constexpr void *operator new(Size, void *) noexcept;
// SSSENGINE_FORCE_INLINE
// constexpr void operator delete(void *, void *) noexcept;
#include <new>

namespace SSSEngine
{
    template<typename T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr RemoveReferenceType<T> &&Move(T &&value) noexcept
    {
        return static_cast<RemoveReferenceType<T> &&>(value);
    }

    template<typename T>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr T &&Forward(RemoveReferenceType<T> &value) noexcept
    {
        return static_cast<T &&>(value);
    }

    template<typename T>
        requires(!IsLValueReference<T>)
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr T &&Forward(RemoveReferenceType<T> &&value) noexcept
    {
        return static_cast<T &&>(value);
    }

    /**
     * @brief Changes the provenance of the pointer. This tells the compiler that
     * the underlying Type changed
     *
     * @tparam T The new Type of the pointer
     * @param ptr The pointer to change
     * @return The same pointer but with the new type associated to it
     */
    template<typename T>
        requires(!IsSameType<const volatile T, const volatile void> && !IsFunction<T>)
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr T *Launder(T *ptr) noexcept
    {
        return __builtin_launder(ptr);
    }

    // FIXME: These construct at functions should check if T is an array type

    /**
     * @brief Constructs an Object of Type T at address
     *
     * @tparam T The type of object to construct
     * @param address The address to construct the object at
     * @param value The value to construct
     */
    template<typename T>
        requires(IsDefaultConstructible<T>)
    SSSENGINE_FORCE_INLINE
    constexpr T *ConstructAt(void *address) noexcept(IsNoThrowDefaultConstructible<T>)
    {
        return ::new(address) T();
    }

    /**
     * @brief Constructs an Object of Type T at address with arguments Args
     *
     * @tparam T The type of object to construct
     * @param address The address to construct the object at
     * @param args The args to use in the constructor
     */
    template<typename T, typename... Args>
        requires(IsConstructible<T, Args...>)
    SSSENGINE_FORCE_INLINE
    constexpr T *ConstructAt(void *address, Args &&...args) noexcept(IsNoThrowConstructible<T, Args...>)
    {
        return ::new(address) T(Forward<Args>(args)...);
    }
} // namespace SSSEngine
