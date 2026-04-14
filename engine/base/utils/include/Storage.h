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
 * @brief Storage
 */

#pragma once

#include "CopyAndMoveTraits.h"
#include "Utility.h"

namespace SSSEngine
{
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
        Storage() = default;

        Storage(const T &value) noexcept(IsNoThrowCopyConstructible<T>) // NOLINT(cppcoreguidelines-pro-type-member-init,
                                                                        // *-explicit-constructor)
            requires(IsCopyConstructible<T>)
        {
            Construct(value);
        }

        Storage(const Storage &storage) noexcept // NOLINT(cppcoreguidelines-pro-type-member-init)
            requires(IsCopyConstructible<T>)
        {
            if(this == &storage)
            {
                return;
            }

            if constexpr(IsBitwiseCopyable<T>)
            {
                MemoryCopy(storage.m_value, m_value, sizeof(T));
            }
            else
            {
                Construct(storage.UnderlyingObject());
            }
        }

        Storage(Storage &&storage) noexcept(IsNoThrowMoveAssignable<T>) // NOLINT(cppcoreguidelines-pro-type-member-init)
            requires(IsMoveConstructible<T>)
        {
            if(this == &storage)
            {
                return;
            }

            Construct(Move(storage.UnderlyingObject()));
        }

        Storage(T &&value) noexcept(IsNoThrowMoveAssignable<T>) // NOLINT(cppcoreguidelines-pro-type-member-init,
                                                                // *-explicit-constructor)
            requires(IsMoveConstructible<T>)
        {
            Construct(Move(value));
        }

        Storage &operator=(const Storage &storage) noexcept(IsNoThrowCopyAssignable<T>)
        {
            if(this == &storage)
            {
                return *this;
            }

            if constexpr(IsBitwiseCopyable<T>)
            {
                MemoryCopy(storage.m_value, m_value, sizeof(T));
            }
            else
            {
                UnderlyingObject() = storage.UnderlyingObject();
            }
            return *this;
        }

        Storage &operator=(const T &value) noexcept(IsNoThrowCopyAssignable<T>)
        {
            UnderlyingObject() = value;
            return *this;
        }

        Storage &operator=(Storage &&storage) noexcept(IsNoThrowMoveAssignable<T>)
        {
            if(this == &storage)
            {
                return *this;
            }

            UnderlyingObject() = Move(storage.UnderlyingObject());
            return *this;
        }

        Storage &operator=(T &&value) noexcept(IsNoThrowMoveAssignable<T>)
        {
            T &object = UnderlyingObject();
            object = Move(value);
            return *this;
        }

        ~Storage()
        {
            UnderlyingObject().~T();
        }

        void Construct(const T &value)
        {
            new(m_value) T(value);
        }

        template<typename... Args>
        void Construct(Args &&...args)
        {
            new(m_value) T(Forward<Args>(args)...);
        }

        void Destroy()
        {
            UnderlyingObject().~T();
        }

        operator T &() // NOLINT(*-explicit-constructor)
        {
            return UnderlyingObject();
        }

        operator const T &() const // NOLINT(*-explicit-constructor)
        {
            return UnderlyingObject();
        }

        private:
        byte m_value[sizeof(T)];

        auto &&UnderlyingObject(this auto &&self) // NOLINT(*-explicit-constructor)
        {
            return *Launder(reinterpret_cast<T *>(Forward<Storage>(self).m_value));
        }
    };
} // namespace SSSEngine
