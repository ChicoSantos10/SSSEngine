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
 * @brief Aligned Storage
 */

#pragma once

#include "Attributes.h"
#include "Byte.h"
#include "Debug.h"
#include "Types.h"
#include "CopyAndMoveTraits.h"
#include "Utility.h"
#include "Address.h"

namespace SSSEngine
{

    // NOLINTBEGIN(*-explicit-constructor, cppcoreguidelines-pro-type-member-init)

    /**
     * @brief A class representing an amount of memory of size and alignment
     *
     * This memory is not default initialized and must be initialized manually.
     * Can hold any type as long as the size and alignment are smaller than the ones defined
     *
     * The class has no knowledge of what type it has so the user must keep track of it if necessary.
     * As such the user must also take care of its lifetime if necessary by calling Destroy if the type is nont
     * trivially destructible.
     *
     * @see Storage for a similar storage that can only hold 1 type or TagUnion for a class that keeps track of the
     * current type stored
     */

    template<SizeType S, SizeType Alignment>
        requires(S > 0 && Alignment > 0)

    class SSSENGINE_DEPRECATED("No reason to use") alignas(Alignment) AlignedStorage
    {
        template<typename T>
        static constexpr bool CanConstruct = sizeof(T) <= S && alignof(T) <= Alignment;

      public:
        /**
         * @brief Default constructs a T
         *
         * @tparam T The type to construct
         */
        template<typename T>
            requires(CanConstruct<T> && IsDefaultConstructible<T>)
        SSSENGINE_FORCE_INLINE
        constexpr T *Construct() noexcept(IsNoThrowDefaultConstructible<T>)
        {
            return BraceConstructAt<T>(AddressOf(m_value[0]));
        }

        /**
         * @brief Constructs a T with constructor arguments
         *
         * @tparam T The type to construct
         * @tparam Args The types to construct with
         * @param args The values to pass to the constructor
         */
        template<typename T, typename... Args>
            requires(CanConstruct<T> && IsConstructible<T, Args...>)
        SSSENGINE_FORCE_INLINE
        constexpr T *Construct(Args &&...args) noexcept(IsNoThrowConstructible<T, Args...>)
        {
            return BraceConstructAt<T>(AddressOf(m_value), Forward<Args>(args)...);
        }

        template<typename T>
            requires(CanConstruct<T> && !IsTriviallyDestructible<T>)
        SSSENGINE_FORCE_INLINE
        constexpr void Destroy() noexcept(IsNoThrowDefaultConstructible<T>)
        {
            Get<T>().~T();
        }

        template<typename T>
            requires(CanConstruct<T>)
        SSSENGINE_FORCE_INLINE
        constexpr T &Get() noexcept
        {
            return *Launder(reinterpret_cast<T *>(m_value));
        }

        template<typename T>
            requires(CanConstruct<T>)
        SSSENGINE_FORCE_INLINE
        constexpr const T &Get() const noexcept
        {
            return *Launder(reinterpret_cast<const T *>(m_value));
        }

      private:
        Byte m_value[S];
    };

    SSSENGINE_STATIC_ASSERT((IsTriviallyCopyable<AlignedStorage<2, 2>>), "Aligned Storage must be trivially copyable");

    // NOLINTEND(*-explicit-constructor, cppcoreguidelines-pro-type-member-init)
} // namespace SSSEngine
