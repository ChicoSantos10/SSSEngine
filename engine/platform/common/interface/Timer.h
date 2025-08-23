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
 * @brief
 */

#pragma once

#include "Attributes.h"
#include "HelperMacros.h"
#include "Types.h"
#include "Debug.h"

// INVESTIGATE: Should be in the platform namespace instead of a time namespace?
namespace SSSEngine::Platform
{

    /**
     * @class Timestamp
     * @brief A number that can be used to determine duration when subtracted to another
     *
     */
    struct Timestamp
    {
        u64 time;
    };

    SSSENGINE_GLOBAL Timestamp operator+(Timestamp lhs, Timestamp rhs)
    {
        return {lhs.time + rhs.time};
    }

    SSSENGINE_GLOBAL Timestamp operator-(Timestamp lhs, Timestamp rhs)
    {
        return {lhs.time - rhs.time};
    }

    /**
     * @brief Gets a timestamp that can be used to calculate the duration of something
     *
     * @return [TODO:return]
     */
    Timestamp GetCurrentTime();

    /**
     * @brief Converts a timestamp to Microseconds duration
     *
     * @param timestamp [TODO:parameter]
     * @return [TODO:return]
     */
    u64 ToMicroSeconds(Timestamp timestamp);
} // namespace SSSEngine::Platform
