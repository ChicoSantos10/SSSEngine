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

#include "Integer.h"
#include "Threading.h"
#include "Attributes.h"
#include "Limits.h"
#include "Types.h"

#include <cerrno>
#include <ctime>
#include <linux/futex.h>
#include <unistd.h>
#include <sys/syscall.h>

namespace SSSEngine::Threading
{
    /**
     * @brief Does a syscall for futex
     *
     * For more information about the parameters read on the linux guide
     *
     * @param uaddr The address to watch
     * @param op The operation to do
     * @param val The value to watch
     * @param timeout The amount of time to wait
     * @param uaddr2 A second address to watch
     * @param val3 The other value to watch
     * @return 0 if sucessful, -1 on fail
     */
    SSSENGINE_FORCE_INLINE
    auto Futex(u32 *uaddr, int op, u32 val, const struct timespec *timeout, u32 *uaddr2, u32 val3)
    {
        return syscall(SYS_futex, uaddr, op, val, timeout, uaddr2, val3);
    }

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
    void Wait(u32 *address, u32 value) noexcept
    {
        if(Futex(address, FUTEX_WAIT_PRIVATE, value, nullptr, nullptr, 0) == -1)
        {
            // INVESTIGATE: Should we return the error? What can we do here?
            switch(errno)
            {
                case EACCES:
                    // No read access to the memory of a futex word
                    break;
                case EAGAIN:
                    // Resource temporarily not available
                    break;
                case EFAULT:
                    // uaddr did not point to a valid userspace address
                    // INVESTIGATE: Since this is always called by the mutex which owns its own memory, it should
                    // always be valid
                    break;
                case EINVAL:
                    // uaddr is not 4-byte aligned
                    break;
                default:
                    // INVESTIGATE: Probably should throw with the error number
                    break;
            }
        }
    }

    /**
     * @brief Notifies one or all threads waiting on address
     *
     * @param addr The address where the threads are waiting
     * @param all If it should notify all threads waiting or just one
     */
    SSSENGINE_FORCE_INLINE
    void Notify(u32 *addr, bool all)
    {
        if(Futex(addr, FUTEX_WAKE_PRIVATE, all ? IntTraits<u32>::Max : 1, nullptr, nullptr, 0) == -1)
        {
            // TODO: Handle errors
        }
    }

    /**
     * @brief Notifies a single thread to wake up
     *
     * @param address The address of the value that the thread is sleeping on
     */
    void NotifyOne(u32 *address) noexcept
    {
        Notify(address, false);
    }

    /**
     * @brief Notifies all threads waiting on address
     *
     * @param address The address of the value that the threads are waiting on
     */
    void NotifyAll(u32 *address) noexcept
    {
        Notify(address, true);
    }

    /**
     * @brief Sleeps for a duration
     *
     * @param duration The amount of nanoseconds to wait
     */
    void PlatformSleep(Nanoseconds duration) noexcept
    {
        SSSENGINE_ASSERT(Seconds(duration).value == 0);

        struct timespec time{
            .tv_sec = 0,
            .tv_nsec = duration.value,
        };
        nanosleep(&time, nullptr);
    }
} // namespace SSSEngine::Threading
