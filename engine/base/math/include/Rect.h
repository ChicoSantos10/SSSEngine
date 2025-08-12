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

/**
 * @file
 * @brief
 */

#pragma once

#include "Concepts.h"

namespace SSSEngine::Math
{
    /**
     * @brief A rect represented by top left position and width and height
     *
     * @tparam T An integer type
     */
    template<IntegralConcept T>
    struct Rect
    {
        T x;
        T y;
        T width;
        T height;
    };
} // namespace SSSEngine::Math
