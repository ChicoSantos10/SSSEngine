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
