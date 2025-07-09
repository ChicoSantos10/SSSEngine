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

#include "Types.h"

struct Color32RGBA
{
    byte R{0}, G{0}, B{0}, A{0};
};

struct ColorRGB
{
    float R{0}, G{0}, B{0};
};

struct ColorRGBA
{
    ColorRGB RGB{.R = 0, .G = 0, .B = 0};
    float A{0};
};
