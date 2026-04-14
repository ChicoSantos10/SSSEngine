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
 * @brief Threading utility
 */

#pragma once

#include "Attributes.h"
#include "TimeDuration.h"
#include "Types.h"

#include <emmintrin.h>

namespace SSSEngine::Threading
{

    /**
     * @brief Performs a pause operation on the CPU. Used to reduce power consumption when waiting
     */
    SSSENGINE_FORCE_INLINE void Pause() noexcept
    {
        _mm_pause();
    }

    /**
     * @brief Yields the execution on the current thread, allowing the OS scheduler to use this thread for other
     * purposes
     */
    void Yield() noexcept;

    /**
     * @brief Waits if value at address is the same as value
     *
     * This operation checks to see if the value pointed to by address still matches the expected value. If so the
     * thread sleeps until woken up by another thread.
     *
     * @see NotifyAll and NotifyOne
     *
     * @param address The address to check
     * @param value The value to check
     */
    void Wait(u32 *address, u32 value) noexcept;

    /**
     * @brief Notifies a single thread to wake up
     *
     * @param address The address of the value that the thread is sleeping on
     */
    void NotifyOne(u32 *address) noexcept;

    /**
     * @brief Notifies all threads waiting on address
     *
     * @param address The address of the value that the threads are waiting on
     */
    void NotifyAll(u32 *address) noexcept;

    /**
     * @brief Sleeps the thread using platform specific sleep withing the granularity and other forms of waiting to
     * ensure that the sleep is as close to duration as possible
     *
     * @param duration The amount in nanoseconds to sleep for
     */
    void Sleep(Nanoseconds duration) noexcept;
} // namespace SSSEngine::Threading
