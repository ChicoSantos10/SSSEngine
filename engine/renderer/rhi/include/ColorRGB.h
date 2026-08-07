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
 * @brief Color structs
 */

#pragma once

#include "Types.h"

namespace SSSEngine::Renderer
{
    /**
     * @class Color32RGBA
     * @brief A color represented by 32 bits
     *
     */
    struct Color32RGBA
    {
        byte r{0}, g{0}, b{0}, a{0};
    };

    /**
     * @class ColorRGB
     * @brief A full representation of a color. Values should be between 0 and 1 but are not clamped
     *
     */
    struct ColorRGB
    {
        float r{0}, g{0}, b{0};
    };

    /**
     * @class ColorRGBA
     * @brief A full representation of a color with alpha. Values should be between 0 and 1 but are not clamped
     *
     */
    struct ColorRGBA
    {
        ColorRGB rgb{.r = 0, .g = 0, .b = 0};
        float a{0};
    };
} // namespace SSSEngine::Renderer
