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

#include "Architecture.h"
#include "Attributes.h"

namespace SSSEngine::System
{
#ifdef SSSENGINE_X64

    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    bool HasAvx2()
    {
        return __builtin_cpu_supports("avx2");
    }

    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    bool HasFma()
    {
        return __builtin_cpu_supports("fma");
    }

    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    bool HasAvx512F()
    {
        return __builtin_cpu_supports("avx512f");
    }

    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    bool HasAvx512BW()
    {
        return __builtin_cpu_supports("avx512bw");
    }

    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    bool HasAvx512DQ()
    {
        return __builtin_cpu_supports("avx512dq");
    }

    SSSENGINE_PURE SSSENGINE_FORCE_INLINE
    bool HasAvx512VL()
    {
        return __builtin_cpu_supports("avx512vl");
    }

#endif
} // namespace SSSEngine::System
