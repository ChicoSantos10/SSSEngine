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

#include "Attributes.h"
#include "HelperMacros.h"
#include <windows.h>

// NOTE: We need to undef this since Windows loves creating macros in PascalCase
#undef GetCurrentTime

#include "Timer.h"
#include "Types.h"

namespace SSSEngine::Platform
{
    const LARGE_INTEGER Frequency = []()
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);

        return freq;
    }();

    Timestamp GetCurrentTime()
    {
        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);

        return {static_cast<u64>(counter.QuadPart)};
    }

    u64 ToMicroSeconds(Timestamp timestamp)
    {
        timestamp.time *= Microseconds;
        // INVESTIGATE:
        // Since this is a division, maybe it's better to return a floating precision number (either 32 or 64 bit)
        // Or increasing the precision
        // Also should we store the inverse of the frequency and multiply instead?
        return timestamp.time / Frequency.QuadPart;
    }
} // namespace SSSEngine::Platform
