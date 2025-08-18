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

#include "Vector.h"

namespace SSSEngine::Core::Gameobjects
{
    /**
     * @class Camera
     * @brief Represents a game camera
     *
     */
    struct Camera
    {
        // TODO: This will need Quaternion instead of rotation. Maybe have transform as well but camera probably wont
        // need a scale
        Math::Float3 position{};
        Math::Float3 rotation{};
    };
} // namespace SSSEngine::Core::Gameobjects
