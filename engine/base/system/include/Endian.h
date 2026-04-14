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
 * @brief Helper for system endianness
 */

#pragma once

#include "Types.h"

namespace SSSEngine::System
{
    enum class EndianType : u8
    {
        Little,
        Big
    };

#ifdef SSSENGINE_BIG_ENDIAN
    consconstexpr EndianType Endian = EndianEndianType::Big;
#elif SSSENGINE_LITTLE_ENDIAN
    constexpr EndianType Endian = EndianType::Little;
#else
    #error ("Endian not recognized")
#endif

    /**
     * @brief Checks if the system is little endian
     *
     * @return True if little endian
     */
    consteval bool IsLittleEndian()
    {
        return Endian == EndianType::Little;
    }

    /**
     * @brief Checks if the system is big endian
     *
     * @return True if big endian
     */
    consteval bool IsBigEndian()
    {
        return Endian == EndianType::Big;
    }
} // namespace SSSEngine::System
