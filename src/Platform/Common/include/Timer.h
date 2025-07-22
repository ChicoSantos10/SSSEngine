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

#pragma once

#include "HelperMacros.h"
#include "Types.h"

namespace SSSEngine
{
    constexpr int Microseconds = static_cast<int>(1e6);
    constexpr int Miliseconds = static_cast<int>(1e3);

    struct Timestamp
    {
        u64 time;
    };

    SSSENGINE_GLOBAL Timestamp operator+(Timestamp lhs, Timestamp rhs) { return {lhs.time + rhs.time}; }
    SSSENGINE_GLOBAL Timestamp operator-(Timestamp lhs, Timestamp rhs) { return {lhs.time - rhs.time}; }

    Timestamp GetCurrentTime();
    u64 ToMicroSeconds(Timestamp timestamp);
} // namespace SSSEngine
