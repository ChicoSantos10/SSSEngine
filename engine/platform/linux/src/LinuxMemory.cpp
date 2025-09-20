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

#include "Debug.h"
#include "Memory.h"
#include "MemorySize.h"

#include <sys/sysinfo.h>
#include <sys/mman.h>

namespace SSSEngine::Platform
{
    MemorySnapshot GetSystemMemoryInfo()
    {
        struct sysinfo info{};

        int success = sysinfo(&info);

        SSSENGINE_ASSERT(success != -1);

        return {.totalSize = {info.totalram * info.mem_unit}, .available = {info.freeram * info.mem_unit}};
    }

    void *AllocateMemory(Math::Bytes bytes, void *startingAddress)
    {
        void *mem = mmap(startingAddress, bytes.value, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        if(mem == MAP_FAILED)
        {
            // TODO: Handle Error
            return nullptr;
        }

        return mem;
    }

    bool FreeMemory(void *address, Math::Bytes size)
    {
        int success = munmap(address, size.value);

        if(success == -1)
        {
            // TODO: Handle Error
            return false;
        }

        return true;
    }
} // namespace SSSEngine::Platform
