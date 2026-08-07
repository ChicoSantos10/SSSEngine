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

#include "PlatformThreadingInternal.h"
#include "Threading.h"
#include "TimeDuration.h"
#include "Timer.h"

namespace SSSEngine::Threading
{

    void Sleep(Nanoseconds duration) noexcept
    {
        Nanoseconds target = Platform::GetCurrentTime() + duration;
        Nanoseconds remaining;

        do
        {
            remaining = target - Platform::GetCurrentTime();
            if(remaining > SleepGranularity)
            {
                PlatformSleep(remaining - SleepGranularity);
            }
            else
            {
                while(Platform::GetCurrentTime() < target)
                {
                    Yield();
                }
                return;
            }
        } while(remaining > 0_ns);
    }
} // namespace SSSEngine::Threading
