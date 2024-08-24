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

#include <intrin.h>
#include <immintrin.h>
#include <ammintrin.h>

// TODO: Compiler specific intrinsics
#ifdef SSSENGINE_MSVC
#elif defined(SSSENGINE_GCC) || defined(SSSENGINE_CLANG)
#endif

// TODO: Other intrinsics
// INVESTIGATE: Naming convention
using Vector128 = __m128;
using Vector256 = __m256;

