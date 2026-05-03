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
 * @brief File for buffer functionality
 */

#pragma once

#include "Attributes.h"
#include "MemorySize.h"
#include "Types.h"

namespace SSSEngine
{
    /**
     * @class Buffer
     * @brief Represents a region of memory
     *
     */
    struct Buffer
    {
        void *address;
        Math::Bytes capacity;
    };

    /**
     * @brief Gets the end of this buffer
     *
     * @param buffer The buffer to get the end
     * @return A pointer to the end of this memory region
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    void *End(const Buffer buffer)
    {
        return reinterpret_cast<byte *>(buffer.address) + buffer.capacity.value;
    }

    /**
     * @brief Gets the end of this buffer
     *
     * @param buffer The buffer to get the end
     * @return A pointer to the end of this memory region
     */
    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    void *Offset(const Buffer buffer, uintptr offset)
    {
        return reinterpret_cast<byte *>(buffer.address) + offset;
    }

} // namespace SSSEngine
