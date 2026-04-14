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

#include "Buffer.h"
#include "MemorySize.h"
#include "Types.h"

namespace SSSEngine::Memory
{
    // INVESTIGATE: Should we just assume 64 always? What should we name this? Do we need both?
#if defined(__GCC_CONSTRUCTIVE_SIZE) && defined(__GCC_DESTRUCTIVE_SIZE)
    SSSENGINE_GLOBAL constexpr Size CacheLineConstructive = __GCC_CONSTRUCTIVE_SIZE;
    SSSENGINE_GLOBAL constexpr Size CacheLineDestructive = __GCC_DESTRUCTIVE_SIZE;
#else
    SSSENGINE_GLOBAL Size CacheLineConstructive = 64;
    SSSENGINE_GLOBAL Size CacheLineDestructive = 64;
#endif

    /**
     * @class MemorySnapshot
     * @brief Represents a memory snapshot
     *
     */
    struct MemorySnapshot
    {
        /**
         * @brief The total size of the system RAM
         */
        // INVESTIGATE: We should probably cache this but probably does not make much of a difference
        Math::Bytes totalSize{0};

        /**
         * @brief Currently available system RAM
         */
        Math::Bytes available{0};
    };

    /**
     * @brief Retrieves a memory snapshot
     * @see MemorySnapshot
     *
     * @return A memory snapshot
     */
    MemorySnapshot GetSystemMemoryInfo();

    /**
     * @brief Gets the size of each page
     *
     * @return The number of bytes each page contains
     */
    Math::Bytes GetSystemPageSize();

    /**
     * @brief Gets the size of each huge page
     *
     * @return The number of bytes each page contains
     */
    Math::Bytes GetSystemHugePageSize();

    /**
     * @brief Reserves bytes size of virtual address space. The memory reserved must be committed before reading or
     * writing into it
     *
     * @param bytes The amount of memory to reserve in bytes
     * @param startingAddress (Optional) The starting address for the reserved memory
     *
     * @return bytes The memory address of the reserved memory. nullptr if it failed
     */
    // TODO: Probably should return an optional/expected<Buffer>
    Buffer ReserveMemory(Math::Bytes size, void *startingAddress = nullptr);

    /**
     * @brief Commits a piece of memory allowing it to be used for reading and writing
     *
     * @param buffer The buffer of reserved of memory to commit
     */
    void CommitMemory(Buffer buffer);

    /**
     * @brief Releases the physical pages of memory while keeping the virtual address space
     *
     * @param address The memory address to release
     * @param length The size of memory to release
     * @return True if it succeeded, false otherwise
     */
    bool ReleaseMemory(Buffer buffer);

    /**
     * @brief Frees memory
     *
     * @param address The memory address to free
     * @param length The size of memory to free
     * @return True if it succeeded, false otherwise
     */
    bool FreeMemory(Buffer buffer);
} // namespace SSSEngine::Memory
