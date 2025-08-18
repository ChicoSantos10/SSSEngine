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
#include "ColorRGB.h"

namespace SSSEngine::Renderer
{
    /**
     * @class Vertex
     * @brief Represents the data needed for to pass the GPU for each vertex of a mesh
     *
     */
    struct Vertex
    {
        Math::Float3 Position{};
        // SSSMath::Float3 Normal{};
        ColorRGBA Color{};
        // INVESTIGATE: Should this always be here? Should we have more classes with different parameters as to save
        // memory?
        // SSSMath::Float2 Uv0;
        // SSSMath::Float2 Uv1;
    };
} // namespace SSSEngine::Renderer
