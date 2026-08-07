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

#include "Attributes.h"
#include "Concepts.h"
#include "Debug.h"
#include "Types.h"
#include "ValueConstant.h"
#include "ValueTraits.h"

namespace SSSEngine
{
    /**
     * @enum Byte
     * @brief Represents a byte. Can only be used for regular byte operations
     *
     */
    enum class Byte : u8
    {
    };

    /**
     * @brief Converts the byte into an integer
     *
     * @tparam Int The type of int to convert to
     * @param byte The byte to convert
     * @return Same as Int(byte)
     */
    template<IntegralConcept Int = u8>
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr Int AsInteger(Byte byte) noexcept
    {
        return static_cast<Int>(byte);
    }

    /**
     * @brief Left shifts the byte by a specified amount
     *
     * @param lhs The byte to shift
     * @param shift The amount to shift
     * @return The byte shifted by amount
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr Byte operator<<(Byte lhs, SizeType shift) noexcept
    {
        return Byte(AsInteger(lhs) << shift);
    }

    /**
     * @brief Right shifts the byte by a specified amount
     *
     * @param lhs The byte to shift
     * @param shift The amount to shift
     * @return The byte shifted by amount
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr Byte operator>>(Byte lhs, SizeType shift) noexcept
    {
        return Byte(AsInteger(lhs) >> shift);
    }

    /**
     * @brief Left shifts the byte by a specified amount
     *
     * @param lhs The byte to shift
     * @param shift The amount to shift
     * @return The byte shifted by amount
     */
    SSSENGINE_FORCE_INLINE
    constexpr Byte &operator<<=(Byte &lhs, SizeType shift) noexcept
    {
        return lhs = lhs << shift;
    }

    /**
     * @brief Right shifts the byte by a specified amount
     *
     * @param lhs The byte to shift
     * @param shift The amount to shift
     * @return The byte shifted by amount
     */
    SSSENGINE_FORCE_INLINE
    constexpr Byte &operator>>=(Byte &lhs, SizeType shift) noexcept
    {
        return lhs = lhs >> shift;
    }

    /**
     * @brief Performs a bitwise Or
     *
     * @param lhs The first byte
     * @param rhs The other byte
     * @return The result of lhs | rhs
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr Byte operator|(Byte lhs, Byte rhs) noexcept
    {
        return Byte(AsInteger(lhs) | AsInteger(rhs));
    }

    /**
     * @brief Performs a bitwise And
     *
     * @param lhs The first byte
     * @param rhs The other byte
     * @return The result of lhs & rhs
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr Byte operator&(Byte lhs, Byte rhs) noexcept
    {
        return Byte(AsInteger(lhs) & AsInteger(rhs));
    }

    /**
     * @brief Performs a XOR
     *
     * @param lhs The first byte
     * @param rhs The other byte
     * @return The result of lhs ^ rhs
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr Byte operator^(Byte lhs, Byte rhs) noexcept
    {
        return Byte(AsInteger(lhs) ^ AsInteger(rhs));
    }

    /**
     * @brief Performs a bitwise OR with assignment
     *
     * @param lhs The first byte
     * @param rhs The other byte
     * @return The result of lhs | rhs
     */
    SSSENGINE_FORCE_INLINE
    constexpr Byte &operator|=(Byte &lhs, Byte rhs) noexcept
    {
        return lhs = lhs | rhs;
    }

    /**
     * @brief Performs a bitwise & with assignment
     *
     * @param lhs The first byte
     * @param rhs The other byte
     * @return The result of lhs & rhs
     */
    SSSENGINE_FORCE_INLINE
    constexpr Byte &operator&=(Byte &lhs, Byte rhs) noexcept
    {
        return lhs = lhs & rhs;
    }

    /**
     * @brief Performs a XOR with assignment
     *
     * @param lhs The first byte
     * @param rhs The other byte
     * @return The result of lhs ^ rhs
     */
    SSSENGINE_FORCE_INLINE
    constexpr Byte operator^=(Byte &lhs, Byte rhs) noexcept
    {
        return lhs = lhs ^ rhs;
    }

    /**
     * @brief Flips all bits
     *
     * @param lhs The byte to flip the bits
     * @return The complement of byte
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    constexpr Byte operator~(Byte lhs) noexcept
    {
        return Byte(~AsInteger(lhs));
    }

    SSSENGINE_STATIC_ASSERT(sizeof(Byte) == 1, "byte must be 1 byte");

} // namespace SSSEngine
