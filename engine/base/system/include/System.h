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
