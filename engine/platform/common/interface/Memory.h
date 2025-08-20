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
 * @brief Interface for interacting with the OS Memory capabilities
 */

#pragma once

#include "Types.h"
#include <new>

namespace SSSEngine::Platform
{
    // INVESTIGATE: Should we just assume 64 always? What should we namme this? Do we need both?
#ifdef __cpp_lib_hardware_interference_size
    SSSENGINE_GLOBAL size CacheLineConstructive = std::hardware_constructive_interference_size;
    SSSENGINE_GLOBAL size CacheLineDestructive = std::hardware_destructive_interference_size;
#elif
    SSSENGINE_GLOBAL size CacheLineConstructive = 64 SSSENGINE_GLOBAL size CacheLineDestructive = 64
#endif

    /**
     * @class MemorySnapshot
     * @brief Represents a memory snapshot
     *
     *
     */
    struct MemorySnapshot
    {
        /**
         * @brief The total size of the system RAM
         */
        u64 TotalSize{0};
        /**
         * @brief Currently available system RAM
         */
        u64 Available{0};
    };

    /**
     * @brief Retrieves a memory snapshot
     * @see MemorySnapshot
     *
     * @return A memory snapshot
     */
    // INVESTIGATE: Should this be in a system header instead
    MemorySnapshot GetSystemMemoryInfo();

    /**
     * @brief Allocates bytes size on the Heap
     *
     * @param bytes The amount of memory to allocate in bytes
     * @param alignment The alignment of the allocated memory
     *
     * @return bytes The memory address of the allocated memory. nullptr if it failed
     */
    // TODO: Probably should return an optional/expected
    void *AllocateMemory(size bytes, size alignment);

    /**
     * @brief Frees memory allocated by AllocateMemory
     * @see AllocateMemory
     *
     * @param address The memory address to free
     * @return True if it succeeded, false otherwise
     */
    bool FreeMemory(void *address);
} // namespace SSSEngine::Platform
