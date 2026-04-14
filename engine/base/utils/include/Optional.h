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

namespace SSSEngine
{

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
        Optional() noexcept : m_exists(false) {}

        Optional(const T &value) noexcept : m_value{value}, m_exists(true) {} // NOLINT(*-explicit-constructor)

        Optional(T &&value) noexcept(IsNoThrowMoveConstructible<T>) : // NOLINT(*-explicit-constructor)
        m_value(Move(value)), m_exists(true)
        {
        }

        Optional(const Optional &opt) noexcept(IsNoThrowCopyConstructible<T>) : m_exists(opt.m_exists)
        {
            if(opt)
            {
                m_value.Construct(opt.m_value);
            }
        }

        Optional(Optional &&opt) noexcept(IsNoThrowMoveConstructible<T>) : m_exists(opt.m_exists)
        {
            if(opt)
            {
                m_value.Construct(Move(opt.m_value));
            }
        }

        ~Optional()
        {
            if(m_exists)
            {
                m_value.Destroy();
            }
        }

        Optional &operator=(const Optional &opt) noexcept(IsNoThrowCopyAssignable<T>)
        {
            if(this == opt)
                return *this;

            if(m_exists)
            {
                m_value.Destroy();
            }
            if(opt)
            {
                m_value = opt.m_value;
            }
            m_exists = opt.m_exists;

            return *this;
        }

        Optional &operator=(Optional &&opt) noexcept(IsNoThrowMoveAssignable<T>)
        {
            if(this == opt)
            {
                return *this;
            }

            if(m_exists)
            {
                m_value.Destroy();
            }
            if(opt)
            {
                m_value = Move(opt.m_value);
            }
            m_exists = opt.m_exists;

            return *this;
        }

        Optional &operator=(const T &val) noexcept(IsNoThrowCopyAssignable<T>)
        {
            if(m_exists)
            {
                m_value.Destroy();
            }

            m_value = val;
            m_exists = true;
            return *this;
        }

        Optional &operator=(T &&val) noexcept(IsNoThrowMoveAssignable<T>)
        {
            if(m_exists)
            {
                m_value.Destroy();
            }

            m_value = Move(val);
            m_exists = true;
            return *this;
        }

        explicit operator bool() noexcept
        {
            return m_exists;
        }

        auto &&operator*(this auto &&self) noexcept
        {
            SSSENGINE_ASSERT(Forward<Optional>(self).m_exists);
            return Forward<Optional>(self).value;
        }

        T *operator->(this auto &self) noexcept
        {
            SSSENGINE_ASSERT(self.m_exists);
            return &self.m_value;
        }

        private:
        Storage<T> m_value{};
        bool m_exists;
    };

} // namespace SSSEngine
