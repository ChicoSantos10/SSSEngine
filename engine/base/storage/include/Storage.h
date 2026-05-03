/*  SSS Engine Copyright (C) 2025  Francisco Santos

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
 * @brief Storage
 */

#pragma once

#include "Attributes.h"
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "Utility.h"
#include "AlignedStorage.h"

namespace SSSEngine
{
    // NOLINTBEGIN(*-explicit-constructor, cppcoreguidelines-pro-type-member-init)

    /**
     * @brief Represents a block of memory big enough to hold 1 T and aligned as T
     *
     * The memory will not be initialized by default and as such is useful to avoid unnecessary object constructions
     *
     * @tparam T The Type to be held
     */
    template<typename T>
    class alignas(T) Storage
    {
      public:
        SSSENGINE_FORCE_INLINE
        constexpr Storage(const T &value) noexcept(IsNoThrowCopyConstructible<T>)
            requires(IsCopyConstructible<T>)
        {
            Construct(value);
        }

        SSSENGINE_FORCE_INLINE
        constexpr Storage(T &&value) noexcept(IsNoThrowMoveConstructible<T>)
            requires(IsMoveConstructible<T>)
        {
            Construct(Move(value));
        }

        SSSENGINE_FORCE_INLINE
        constexpr Storage &operator=(const T &value) noexcept(IsNoThrowCopyAssignable<T>)
            requires(IsCopyAssignable<T>)
        {
            Construct(value);
            return *this;
        }

        SSSENGINE_FORCE_INLINE
        constexpr Storage &operator=(T &&value) noexcept(IsNoThrowMoveAssignable<T>)
            requires(IsMoveAssignable<T>)
        {
            UnderlyingObject() = Move(value);
            return *this;
        }

        Storage() = default;
        constexpr Storage(const Storage &storage) = default;
        constexpr Storage(Storage &&storage) = default;
        constexpr Storage &operator=(const Storage &storage) = default;
        constexpr Storage &operator=(Storage &&storage) = default;
        constexpr ~Storage() = default;

        SSSENGINE_FORCE_INLINE
        constexpr Storage(const Storage &storage) noexcept(IsNoThrowCopyConstructible<T>)
            requires(IsCopyConstructible<T> && !IsTriviallyCopyable<T>)
        {
            Construct(storage.UnderlyingObject());
        }

        SSSENGINE_FORCE_INLINE
        constexpr Storage(Storage &&storage) noexcept(IsNoThrowMoveConstructible<T>)
            requires(IsMoveConstructible<T> && !IsTriviallyMoveConstructible<T>)
        {
            Construct(storage.UnderlyingObject());
        }

        SSSENGINE_FORCE_INLINE
        constexpr Storage &operator=(const Storage &storage) noexcept(IsNoThrowCopyAssignable<T>)
            requires(IsCopyAssignable<T> && !IsTriviallyCopyable<T>)
        {
            UnderlyingObject() = storage.UnderlyingObject();
            return *this;
        }

        SSSENGINE_FORCE_INLINE
        constexpr Storage &operator=(Storage &&storage) noexcept(IsNoThrowMoveAssignable<T>)
            requires(IsMoveAssignable<T> && !IsTriviallyMoveAssignable<T>)
        {
            UnderlyingObject() = Move(storage.UnderlyingObject());
            return *this;
        }

        SSSENGINE_FORCE_INLINE
        constexpr T *Construct() noexcept(IsNoThrowDefaultConstructible<T>)
            requires(IsDefaultConstructible<T>)
        {
            return m_value.template Construct<T>();
        }

        template<typename... Args>
        SSSENGINE_FORCE_INLINE
        constexpr T *Construct(Args &&...args) noexcept(IsNoThrowConstructible<T, Args...>)
            requires(IsConstructible<T, Args...>)
        {
            return m_value.template Construct<T>(Forward<Args...>(args)...);
        }

        SSSENGINE_FORCE_INLINE
        constexpr void Destroy() noexcept(IsNoThrowDestructible<T>)
            requires(IsDestructible<T> && !IsTriviallyDestructible<T>)
        {
            m_value.template Destroy<T>();
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto &&Get(this Self &&self) noexcept
        {
            return Forward<Self>(self).UnderlyingObject();
        }

        /**
         * @brief Conversion operator from Storage of T to T
         *
         * @return The underlying T Object
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr operator T &()
        {
            return UnderlyingObject();
        }

        /**
         * @brief Conversion operator from Storage of T to T
         *
         * @return The underlying const T Object
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr operator const T &() const
        {
            return UnderlyingObject();
        }

      private:
        AlignedStorage<sizeof(T), alignof(T)> m_value;

        /**
         * @brief Get's the underlying T Object
         *
         * @return A reference to the underlying T Object
         */
        template<typename Self>
        SSSENGINE_FORCE_INLINE
        constexpr auto &&UnderlyingObject(this Self &&self)
        {
            return Forward<Self>(self).m_value.template Get<T>();
        }
    };

    // NOLINTEND(*-explicit-constructor, cppcoreguidelines-pro-type-member-init)
} // namespace SSSEngine
