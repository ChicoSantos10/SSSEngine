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
 * @brief Thread functions implementation
 */

#pragma once

#include "Thread.h"
#include "PlatformThreadingInternal.h"

#include <pthread.h>

namespace SSSEngine::Threading
{
    void *LinuxThreadStart(void *arg) noexcept
    {
        u64 id = pthread_self();
        CurrentThread = {.handle = id, .id = id};
        auto error = ThreadStart();
        return nullptr;
    }

    Thread CreateThread() noexcept
    {
        pthread_t newThread; // NOLINT(cppcoreguidelines-init-variables)

        // TODO: Since attributes should probably be the same for every thread, we can initialize it at the beginning
        // and only cleanup on exit
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        auto failed = pthread_create(&newThread, &attr, LinuxThreadStart, nullptr);

        pthread_attr_destroy(&attr);

        if(failed)
        {
            // TODO: Handle error
        }

        return {.handle = newThread, .id = newThread};
    }
} // namespace SSSEngine::Threading
