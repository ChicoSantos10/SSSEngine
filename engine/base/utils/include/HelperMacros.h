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

#define SSSENGINE_WIDE_STRING_(x) L ## x
#define SSSENGINE_WIDE_STRING(x) SSSENGINE_WIDE_STRING_(#x)
#define SSSENGINE_WIDE(x) SSSENGINE_WIDE_STRING_(x)

#define SSSENGINE_PRAGMA(x) _Pragma(#x)

#define SSSENGINE_GLOBAL inline
#define SSSENGINE_INTERNAL static
#define SSSENGINE_FUNCTION_LOCAL static

#define SSSENGINE_LIB(library) SSSENGINE_PRAGMA(comment(lib, #library))
