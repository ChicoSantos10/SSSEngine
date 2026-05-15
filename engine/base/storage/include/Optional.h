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

#include "CopyAndMoveTraits.h"
#include "Debug.h"
#include "Storage.h"
#include "Utility.h"
#include "Attributes.h"

namespace SSSEngine
{
    // NOLINTBEGIN(*-explicit-constructor)

    /**
     * @brief Represents an optional object. This means that it may or may not have a T inside.
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
    class Optional
    {
      public:
        SSSENGINE_FORCE_INLINE
        Optional(const T &value) noexcept(IsNoThrowCopyConstructible<T>)
            requires(IsCopyConstructible<T>)
            : m_value{value}, m_exists(true)
        {
        }

        SSSENGINE_FORCE_INLINE
        Optional(T &&value) noexcept(IsNoThrowMoveConstructible<T>)
            requires(IsMoveConstructible<T>)
            : m_value(Move(value)), m_exists(true)
        {
        }

        SSSENGINE_FORCE_INLINE
        Optional &operator=(const T &val) noexcept(IsNoThrowCopyAssignable<T>)
            requires(IsCopyAssignable<T>)
        {
            if constexpr(!IsTriviallyDestructible<T>)
            {
                if(m_exists)
                {
                    m_value.Destroy();
                }
            }

            m_value = val;
            m_exists = true;
            return *this;
        }

        SSSENGINE_FORCE_INLINE
        Optional &operator=(T &&val) noexcept(IsNoThrowMoveAssignable<T>)
            requires(IsMoveAssignable<T>)
        {
            if constexpr(!IsTriviallyDestructible<T>)
            {
                if(m_exists)
                {
                    m_value.Destroy();
                }
            }

            m_value = Move(val);
            m_exists = true;
            return *this;
        }

        SSSENGINE_FORCE_INLINE
        Optional(const Optional &opt) noexcept(IsNoThrowCopyConstructible<T>)
            requires(!IsTriviallyCopyable<T>)
            : m_exists(opt.m_exists)
        {
            if(opt)
            {
                m_value = opt.m_value;
            }
        }

        SSSENGINE_FORCE_INLINE
        Optional(Optional &&opt) noexcept(IsNoThrowMoveConstructible<T>)
            requires(!IsTriviallyCopyable<T>)
            : m_exists(opt.m_exists)
        {
            if(opt)
            {
                m_value = Move(opt.m_value);
            }
        }

        SSSENGINE_FORCE_INLINE
        ~Optional()
            requires(!IsTriviallyDestructible<T>)
        {
            if(m_exists)
            {
                m_value.Destroy();
            }
        }

        SSSENGINE_FORCE_INLINE
        Optional &operator=(const Optional &opt) noexcept(IsNoThrowCopyAssignable<T>)
            requires(!IsTriviallyCopyable<T> && IsCopyAssignable<T>)
        {
            if(!opt)
            {
                if(m_exists)
                {
                    if constexpr(!IsTriviallyDestructible<T>)
                    {
                        m_value.Destroy();
                    }
                    m_exists = false;
                }
                return *this;
            }

            if(m_exists)
            {
                m_value = opt.m_value;
            }
            else
            {
                m_value.Construct(opt.m_value.Get());
            }
            m_exists = true;

            return *this;
        }

        SSSENGINE_FORCE_INLINE
        Optional &operator=(Optional &&opt) noexcept(IsNoThrowMoveAssignable<T>)
            requires(!IsTriviallyMoveAssignable<T> && IsMoveAssignable<T>)
        {
            if(!opt)
            {
                if(m_exists)
                {
                    if constexpr(!IsTriviallyDestructible<T>)
                    {
                        m_value.Destroy();
                    }
                    m_exists = false;
                }
                return *this;
            }

            if(m_exists)
            {
                m_value = Move(opt.m_value);
            }
            else
            {
                m_value.Construct(Move(opt.m_value.Get()));
            }
            m_exists = true;

            return *this;
        }

        Optional() = default;
        Optional(const Optional &) = default;
        Optional(Optional &&) = default;
        ~Optional() = default;
        Optional &operator=(const Optional &) = default;
        Optional &operator=(Optional &&) = default;

        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        explicit operator bool() noexcept
        {
            return m_exists;
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        auto &&operator*(this Self &&self) noexcept
        {
            SSSENGINE_ASSERT(Forward<Self>(self).m_exists);
            return Forward<Self>(self).m_value.Get();
        }

        template<typename Self>
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        T *operator->(this Self &&self) noexcept
        {
            SSSENGINE_ASSERT(Forward<Self>(self.m_exists));
            return &Forward<Self>(self).m_value.Get();
        }

      private:
        Storage<T> m_value{};
        bool m_exists{};
    };

    // TODO: Specialize pointers and references

    // NOLINTEND(*-explicit-constructor)

} // namespace SSSEngine
