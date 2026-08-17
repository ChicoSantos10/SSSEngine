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

#include "Address.h"
#include "Attributes.h"
#include "CopyAndMoveTraits.h"
#include "Utility.h"

namespace SSSEngine
{
    // NOLINTBEGIN(*-explicit-constructor, cppcoreguidelines-pro-type-member-init)

    template<typename T>
    union StorageData
    {
        T data;

        StorageData() = default;
        StorageData(const StorageData &) = default;
        StorageData(StorageData &&) = default;
        StorageData &operator=(const StorageData &) = default;
        StorageData &operator=(StorageData &&) = default;
        ~StorageData() = default;

        StorageData()
            requires(!IsTriviallyDefaultConstructible<T>)
        {
        }

        ~StorageData()
            requires(!IsTriviallyDestructible<T>)
        {
        }
    };

    /**
     * @brief Wrapper around an object of Type T
     *
     * The memory will not be initialized by default and as such is useful to avoid unnecessary object constructions
     *
     * @important This object does not track lifetimes and as such should be used from other objects or manually keep
     * track of lifetime. This object is trivially destructible even if T is not. This means that the user must manually
     * destroy T otherwise resources will be leaked.
     *
     * @tparam T The Type to be held
     */
    template<typename T>
    class Storage
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

        template<typename... Args>
        SSSENGINE_FORCE_INLINE
        constexpr Storage(Args... args) noexcept(IsNoThrowConstructible<T, Args...>)
            requires(IsConstructible<T, Args...>)
        {
            Construct(Forward<Args>(args)...);
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
            Construct(storage.Get());
        }

        SSSENGINE_FORCE_INLINE
        constexpr Storage(Storage &&storage) noexcept(IsNoThrowMoveConstructible<T>)
            requires(IsMoveConstructible<T> && !IsTriviallyMoveConstructible<T>)
        {
            Construct(Move(storage.Get()));
        }

        template<typename... Args>
        SSSENGINE_FORCE_INLINE
        constexpr T *Construct(Args &&...args) noexcept(IsNoThrowConstructible<T, Args...>)
            requires(IsConstructible<T, Args...>)
        {
            return BraceConstructAt<T>(AddressOf(m_storage.data), Forward<Args>(args)...);
        }

        SSSENGINE_FORCE_INLINE
        constexpr void Destroy() noexcept
        {
        }

        SSSENGINE_FORCE_INLINE
        constexpr void Destroy() noexcept(IsNoThrowDestructible<T>)
            requires(IsDestructible<T> && !IsTriviallyDestructible<T>)
        {
            DestroyAt<T>(AddressOf(m_storage.data));
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr T &Get() & noexcept
        {
            return m_storage.data;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr const T &Get() const & noexcept
        {
            return m_storage.data;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr T &&Get() && noexcept
        {
            return Move(m_storage.data);
        }

        constexpr const T &&Get() const && = delete; // NOLINT(modernize-use-nodiscard, Its deleted no use marking
                                                     // nodiscard)

        template<typename Self>
        constexpr operator T(this Self &&self) noexcept
        {
            return Forward<Self>(self).Get();
        }

      private:
        StorageData<T> m_storage;
    };

    // INVESTIGATE: What happens if the type T is an array of not trivial types? Is it a memory leak?

    // NOLINTEND(*-explicit-constructor, cppcoreguidelines-pro-type-member-init)
} // namespace SSSEngine
