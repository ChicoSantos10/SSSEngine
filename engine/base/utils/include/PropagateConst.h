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
#include "QualifierTraits.h"
#include "Utility.h"

namespace SSSEngine::Utils
{
    /**
     * @brief Propagates the constness to the variable
     *
     * @tparam T The type of the variable to propagate the constness
     */
    template<PointerConcept T>
    struct PropagateConst
    {
        using ElementType = RemovePointerType<T>;

        // NOLINTBEGIN(google-explicit-constructor)
        constexpr PropagateConst() noexcept = default;
        constexpr PropagateConst(T ptr) noexcept : m_ptr{ptr} {};
        constexpr PropagateConst(const ElementType *ptr) noexcept :
        m_ptr{const_cast<T>(ptr)} {}; // NOLINT(*-const-cast, We still keep const correctness)

        constexpr auto &&operator*(this auto &&self) noexcept
        {
            return *Forward<decltype(self)>(self).m_ptr;
        }

        constexpr auto &&operator->(this auto &&self) noexcept
        {
            return Forward<decltype(self)>(self).m_ptr;
        }

        constexpr operator ElementType *() noexcept
        {
            return m_ptr;
        }

        constexpr operator const ElementType *() const noexcept
        {
            return m_ptr;
        }

        private:
        T m_ptr = nullptr;

        // NOLINTEND(google-explicit-constructor)
    };
} // namespace SSSEngine::Utils
