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
 * @brief Optional
 */

#pragma once

#include "ConversionTraits.h"
#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "InitializerList.h"
#include "QualifierTraits.h"
#include "Storage.h"
#include "Swap.h"
#include "Utility.h"
#include "Attributes.h"

namespace SSSEngine
{
    // NOLINTBEGIN(*-explicit-constructor)

    /**
     * @brief Represents an object that may or may not have a T inside.
     *
     * To access the object make sure it exists first like this:
     *
     * @code
     * Optional<T> myOpt = GetOptional();
     * if (myOpt)
     * {
     *  // Object exists, you can do whatever with it
     *  T value = *myOpt;
     * }
     * else
     * {
     *  // Object does not exist, handle if needed
     * }
     * @endcode
     *
     * @tparam T The Type of object to hold
     */
    template<typename T>
    class Optional final : private Storage<T>
    {
        using Base = Storage<T>;
        using Base::Construct;
        using Base::Get;

      public:
        constexpr Optional() : m_exists(false) {};

        SSSENGINE_FORCE_INLINE
        constexpr Optional(const T &value) noexcept(IsNoThrowCopyConstructible<T>)
            requires(IsCopyConstructible<T>)
            : Base(value), m_exists(true)
        {
        }

        SSSENGINE_FORCE_INLINE
        constexpr Optional(T &&value) noexcept(IsNoThrowMoveConstructible<T>)
            requires(IsMoveConstructible<T>)
            : Base(Move(value)), m_exists(true)
        {
        }

        template<typename U>
            requires(!IsSameType<U, T> && IsConstructible<T, const U &>)
        SSSENGINE_FORCE_INLINE
        constexpr explicit(!IsConvertible<U &&, T>) Optional(U &&value) noexcept(IsNoThrowConstructible<T, U>) :
            Base(Forward<U>(value)), m_exists(true)
        {
        }

        SSSENGINE_FORCE_INLINE
        constexpr Optional &operator=(const T &val) noexcept(IsNoThrowCopyConstructible<T> && IsNoThrowCopyAssignable<T>)
            requires(IsCopyAssignable<T> && IsCopyConstructible<T>)
        {
            if(m_exists)
            {
                Get() = val;
            }
            else
            {
                Construct(val);
                m_exists = true;
            }

            return *this;
        }

        SSSENGINE_FORCE_INLINE
        constexpr Optional &operator=(T &&val) noexcept(IsNoThrowMoveConstructible<T> && IsNoThrowMoveAssignable<T>)
            requires(IsMoveConstructible<T> && IsMoveAssignable<T>)
        {
            if(m_exists)
            {
                Get() = Move(val);
            }
            else
            {
                Construct(Move(val));
                m_exists = true;
            }

            return *this;
        }

        SSSENGINE_FORCE_INLINE
        constexpr Optional(const Optional &opt) noexcept(IsNoThrowCopyConstructible<T>)
            requires(IsCopyConstructible<T> && !IsTriviallyCopyConstructible<T>)
            : m_exists(opt.m_exists)

        {
            if(opt)
            {
                Construct(opt.Get());
            }
        }

        SSSENGINE_FORCE_INLINE
        constexpr Optional(Optional &&opt) noexcept(IsNoThrowMoveConstructible<T>)
            requires(IsMoveConstructible<T> && !IsTriviallyMoveConstructible<T>)
            : m_exists(opt.m_exists)
        {
            if(opt)
            {
                Construct(Move(opt.Get()));
                opt.m_exists = false;
            }
        }

        SSSENGINE_FORCE_INLINE
        constexpr ~Optional()
            requires(!IsTriviallyDestructible<T>)
        {
            if(m_exists)
            {
                Destroy();
            }
        }

        SSSENGINE_FORCE_INLINE
        constexpr Optional &
        operator=(const Optional &opt) noexcept(IsNoThrowCopyConstructible<T> && IsNoThrowCopyAssignable<T>)
            requires(IsCopyConstructible<T> && IsCopyAssignable<T> && !IsTriviallyCopyAssignable<T>)
        {
            if(!opt)
            {
                if(m_exists)
                {
                    if constexpr(!IsTriviallyDestructible<T>)
                    {
                        Base::Destroy();
                    }
                    m_exists = false;
                }
                return *this;
            }

            *this = opt.Get();

            return *this;
        }

        SSSENGINE_FORCE_INLINE
        constexpr Optional &operator=(Optional &&opt) noexcept(IsNoThrowMoveConstructible<T> && IsNoThrowMoveAssignable<T>)
            requires(IsMoveConstructible<T> && IsMoveAssignable<T> && !IsTriviallyMoveAssignable<T>)
        {
            if(!opt)
            {
                if(m_exists)
                {
                    if constexpr(!IsTriviallyDestructible<T>)
                    {
                        Base::Destroy();
                    }
                    m_exists = false;
                }
                return *this;
            }

            *this = Move(opt.Get());
            opt.m_exists = false;

            return *this;
        }

        // NOLINTEND(*-explicit-constructor)

        template<typename U>
            requires IsConstructible<T, const U &>
        SSSENGINE_FORCE_INLINE
        constexpr explicit(!IsConvertible<U &&, T>)
            Optional(const Optional<U> &opt) noexcept(IsNoThrowConstructible<T, const U &>) :
            m_exists(opt.m_exists)
        {
            if(opt)
            {
                Construct(opt.Value());
            }
        }

        template<typename U>
            requires IsConstructible<T, const U &>
        constexpr explicit(!IsConvertible<U &&, T>)
            Optional(Optional<U> &&opt) noexcept(IsNoThrowConstructible<T, const U &>) :
            m_exists(opt.m_exists)
        {
            if(opt)
            {
                Construct(Move(opt.Value()));
                opt.m_exists = false;
            }
        }

        template<typename... Args>
            requires IsConstructible<T, Args...>
        constexpr explicit Optional(InPlaceType, Args &&...args) noexcept(IsNoThrowConstructible<T, Args...>) :
            Base(Forward<Args>(args)...), m_exists(true)
        {
        }

        template<typename U, typename... Args>
            requires IsConstructible<T, Args...>
        constexpr explicit Optional(InPlaceType,
                                    InitializerList<U> list,
                                    Args &&...args) noexcept(IsNoThrowConstructible<T, Args...>) :
            Base(list, Forward<Args>(args)...), m_exists(true)
        {
        }

        Optional(const Optional &) = default;
        Optional(Optional &&) = default;
        ~Optional() = default;
        Optional &operator=(const Optional &) = default;
        Optional &operator=(Optional &&) = default;

        template<typename... Args>
            requires IsConstructible<T, Args...>
        constexpr T &Emplace(Args &&...args)
        {
            if constexpr(!IsTriviallyDestructible<T>)
            {
                if(m_exists)
                {
                    Base::Destroy();
                }
            }
            Construct(Forward<Args>(args)...);
            m_exists = true;

            return Get();
        }

        template<typename U, typename... Args>
            requires IsConstructible<T, InitializerList<U> &, Args...>
        constexpr T &Emplace(InitializerList<U> list, Args &&...args)
        {
            if constexpr(!IsTriviallyDestructible<T>)
            {
                if(m_exists)
                {
                    Base::Destroy();
                }
            }
            Construct(list, Forward<Args>(args)...);
            m_exists = true;

            return Get();
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr explicit operator bool() const noexcept
        {
            return m_exists;
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr decltype(auto) operator*(this Self &&self) noexcept
        {
            SSSENGINE_ASSERT(Forward<Self>(self).m_exists);
            return Forward<Self>(self).Get();
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr T *operator->(this Self &&self) noexcept
        {
            SSSENGINE_ASSERT(Forward<Self>(self).m_exists);
            return &Forward<Self>(self).Get();
        }

        template<typename Default = RemoveCVReferenceType<T>>
            requires IsConvertible<Default, T>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr T ValueOr(Default &&def) const &
        {
            if(m_exists)
            {
                return Get();
            }

            return static_cast<T>(Forward<Default>(def));
        }

        template<typename Default = RemoveCVReferenceType<T>>
            requires IsConvertible<Default, T>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr T ValueOr(Default &&def) &&
        {
            if(m_exists)
            {
                return Move(Get());
            }

            return static_cast<T>(Forward<Default>(def));
        }

        SSSENGINE_FORCE_INLINE
        constexpr void Swap(Optional &other) noexcept(IsNoThrowSwappable<T>)
            requires(IsSwappable<T>)
        {
            Swap(this, other);
        }

        template<typename Self>
        SSSENGINE_FORCE_INLINE
        constexpr decltype(auto) Value(this Self &&self) noexcept
        {
            return Forward<Self>(self).Get();
        }

        template<typename U>
        constexpr friend void Swap(Optional<U> &rhs, Optional<U> &lhs) noexcept(IsNoThrowSwappable<T>)
            requires(IsSwappable<T>)
        {
            using Utility::Swap;

            if(rhs.m_exists && lhs.m_exists)
            {
                Swap(rhs.Get(), lhs.Get());
            }
            else if(rhs.m_exists)
            {
                lhs.Construct(Move(rhs));
            }
            else if(lhs.m_exists)
            {
                rhs.Construct(Move(lhs));
            }
        }

        SSSENGINE_FORCE_INLINE
        constexpr void Destroy()
        {
            SSSENGINE_ASSERT(m_exists);

            Base::Destroy();
            m_exists = false;
        }

      private:
        bool m_exists;
    };

    template<typename T>
    class Optional<T *>
    {
        SSSENGINE_NOT_IMPLEMENTED;

        using Type = T *;

      private:
        T *m_value;
    };

    template<typename T>
    class Optional<T &> final
    {
      public:
        constexpr Optional() = default;
        constexpr Optional(const Optional &) = default;
        constexpr Optional(Optional &&) = default;
        constexpr Optional &operator=(const Optional &) = default;
        constexpr Optional &operator=(Optional &&) = default;
        constexpr ~Optional() = default;

        // NOLINTNEXTLINE(google-explicit-constructor)
        constexpr Optional(T &value) noexcept : m_value(&value) {}

        SSSENGINE_FORCE_INLINE
        constexpr Optional &operator=(T &value) noexcept
        {
            m_value = &value;
            return *this;
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto &&operator*(this Self &&self) noexcept
        {
            SSSENGINE_ASSERT(Forward<Self>(self).m_value != nullptr);
            return *Forward<Self>(self).m_value;
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr T *operator->(this Self &&self) noexcept
        {
            SSSENGINE_ASSERT(Forward<Self>(self).m_value != nullptr);
            return Forward<Self>(self).m_value;
        }

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr explicit operator bool() const noexcept
        {
            return m_value != nullptr;
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr auto &&Value(this Self &&self) noexcept
        {
            return *Forward<Self>(self);
        }

        SSSENGINE_FORCE_INLINE
        constexpr void Swap(Optional &other) noexcept
        {
            Swap(*this, other);
        }

        SSSENGINE_FORCE_INLINE
        constexpr void Reset() noexcept
        {
            m_value = nullptr;
        }

        SSSENGINE_FORCE_INLINE
        friend void Swap(Optional &rhs, Optional &lhs) noexcept
        {
            Utility::Swap(rhs.m_value, lhs.m_value);
        }

      private:
        T *m_value;
    };

} // namespace SSSEngine
