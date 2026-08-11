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

/*
 * @file
 * @brief Implementation of Linux Memory functions
 *
 */

#include "Buffer.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "Memory.h"
#include "MemorySize.h"

#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <unistd.h>

namespace SSSEngine::Memory
{
    MemorySnapshot GetSystemMemoryInfo()
    {
        struct sysinfo info{};

        int success = sysinfo(&info);

        SSSENGINE_ASSERT(success != -1);

        return {.totalSize = {info.totalram * info.mem_unit}, .available = {info.freeram * info.mem_unit}};
    }

    Math::Bytes GetSystemPageSize()
    {
        SSSENGINE_FUNCTION_LOCAL Math::Bytes pageSize = {static_cast<Math::Bytes::ValueType>(getpagesize())};
        return pageSize;
    }

    Math::Bytes GetSystemHugePageSize()
    {
        // TODO: Scan the /proc/meminfo for the correct value
        return Math::Bytes(2_MiB);
    }

    Buffer ReserveMemory(Math::Bytes size, void *startingAddress)
    {
        void *mem = mmap(startingAddress, size.value, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if(mem == MAP_FAILED)
        {
            // TODO: Handle Error
            return {.address = nullptr, .capacity = 0_B};
        }

        return {.address = mem, .capacity = size};
    }

    void CommitMemory(Buffer buffer)
    {
        mprotect(buffer.address, buffer.capacity.value, PROT_READ | PROT_WRITE);
    }

    bool ReleaseMemory(Buffer buffer)
    {
        int success = madvise(buffer.address, buffer.capacity.value, MADV_DONTNEED);

        if(success == -1)
        {
            // TODO: Handle Error
            return false;
        }

        return true;
    }

    bool FreeMemory(Buffer buffer)
    {
        int success = munmap(buffer.address, buffer.capacity.value);

        if(success == -1)
        {
            // TODO: Handle Error
            return false;
        }

        return true;
    }
} // namespace SSSEngine::Memory
