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
 * @brief Serves as the application allocator. Responsible for giving other allocators their memory buffers by
 * requesting from the OS
 */

#pragma once

#include "Buffer.h"
#include "MemorySize.h"

namespace SSSEngine::Memory
{
    // TODO: Reserve should allow Huge Pages

    /**
     * @brief Reserves, but does not commit the requested pages of memory.
     * @see The reserved memory can then be asked with Request
     *
     * @param bytes The number of pages to reserve
     */
    void Reserve(u32 pages);

    /**
     * @brief Requests memory from the application. If not enough memory available will try to request more from the OS
     *
     * @see Must use Release to release the memory given by this function
     *
     * @param size The amount of bytes to request
     */
    Buffer Request(Math::Bytes bytes);

    /**
     * @brief Releases to the application memory previously given by Request
     *
     * @see Use Request to get a buffer
     *
     * @param buffer The buffer to release
     */
    void Release(Buffer buffer);

    /*
      INVESTIGATE: Since we can allocate essentially infinite memory, we can just allocate far more virtual memory than
      what we will use. We can then allocate page size and commit it when needed. When freeing we can just release the
      memory.
        - PROS: Simple we just increment the buffer id or move the pointer and we do not need to worry about
        - CONS: More commits and releases of memory
        - ALTERNATIVE: Have a linked list or a u64/32 flags of available buffers. When releasing add it to the queue.
      Always keep a minimum amount of available buffers, release memory when we go over it. The minimum should be
      relative to the system max memory. This allocator can allocate the first page on program load and use a fixed
      amount to store any relevant data that needs to live on the heap

      INVESTIGATE: Using guard pages in debug mode to prevent memory write across pages. We can do that by leaving an
      extra address space page after each memory buffer with PROT_NONE or PAGE_NO_ACCESS

      INVESTIGATE: TLSF Allocator for general purpose heap allocations, Arenas for known lifetime data, Pools for
      fixed-size with known lifetime data, Frame/Stack or DualFrame for data that lives for a single frame. The
      applicaton allocator for giving other allocators their memory, a shared allocator for thread safe operations on
      data that can be shared.

      INVESTIGATE: What is the best allocator strategy for entities?
    */

} // namespace SSSEngine::Memory
