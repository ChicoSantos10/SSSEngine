/*  SSS Engine
    Copyright (C) 2024  Francisco Santos

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <windows.h>
#include "Timer.h"
#include "Types.h"

namespace SSSEngine
{
    Timestamp GetCurrentTime()
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);

        return {static_cast<u64>(counter.QuadPart)};
    }

    u64 ToMicroSeconds(Timestamp timestamp)
    {
        // TODO: This value does not change so should be cached instead
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);

        timestamp.time *= Microseconds;
        // INVESTIGATE:
        // Since this is a division, maybe it's better to return a floating precision number (either 32 or 64 bit)
        return timestamp.time / frequency.QuadPart;
    }
} // namespace SSSEngine
