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
 * @brief Mutex
 */

#pragma once

#include "Atomics.h"
#include "Attributes.h"
#include "Debug.h"
#include "PowerSaving.h"
#include "Types.h"

namespace SSSEngine::Threading
{

    /**
     * @class Mutex
     * @brief Synchronization primitive that prevents multiple threads from accessing the same data at the same time
     *
     */
    class Mutex
    {
        public:
        /**
         * @brief Tries to obtain the lock and if failed waits until notified by another thread to try again
         */
        void Lock() noexcept
        {
            while(!TryLock())
            {
                m_flag.Wait(Locked);
            }
        }

        /**
         * @brief Continues to try to obtain the lock by only waiting a few cycles each retry.
         *
         * @important Only use when you know that the thread will only ever have to wait a few cycles before obtaining
         * the lock!
         *
         * @see Pause
         */
        void SpinLock() noexcept
        {
            while(!TryLock())
            {
                Pause();
            }
        }

        /**
         * @brief Continues to try to obtain the lock by yielding the thread
         *
         * @important Use when the wait is short (microseconds)
         *
         * @see Yield
         */
        void YieldLock() noexcept
        {
            while(!TryLock())
            {
                Yield();
            }
        }

        /**
         * @brief Tries to acquire the lock
         *
         * @return True if successful
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE bool TryLock() noexcept
        {
            return m_flag.Exchange(Locked, MemoryOrder::Acquire) == 0;
        }

        /**
         * @brief Unlocks and notifies one waiting thread
         */
        void Unlock() noexcept
        {
            SSSENGINE_ASSERT(m_flag.Load(MemoryOrder::Relaxed) == Locked);

            // INVESTIGATE: Should we Notify even when no threads are waiting?
            m_flag.Store(Unlocked, MemoryOrder::Release);
            m_flag.NotifyOne();
        }

        private:
        Atomic<u32, MemoryOrder::Relaxed> m_flag;

        enum State : u8
        {
            Unlocked = 0,
            Locked = 1,
        };
    };

    enum class LockStrategy : u8
    {
        Lock,
        SpinLock,
        YieldLock,
    };

    template<LockStrategy Strategy>
    class MutexLock
    {
        public:
        explicit MutexLock(Mutex &mutex) : m_mutex(mutex)
        {
            if constexpr(Strategy == LockStrategy::Lock)
            {
                m_mutex.Lock();
            }
            else if constexpr(Strategy == LockStrategy::SpinLock)
            {
                m_mutex.SpinLock();
            }
            else if constexpr(Strategy == LockStrategy::YieldLock)
            {
                m_mutex.YieldLock();
            }
        }

        ~MutexLock()
        {
            m_mutex.Unlock();
        }

        MutexLock(const MutexLock &) = delete;
        MutexLock(MutexLock &&) = default;
        MutexLock &operator=(const MutexLock &) = delete;
        MutexLock &operator=(MutexLock &&) = delete;

        private:
        Mutex &m_mutex; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
    };

} // namespace SSSEngine::Threading
