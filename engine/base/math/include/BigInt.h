/**
 * @file
 * @brief
 */

#pragma once

namespace SSSEngine::Math
{
#ifdef __SIZEOF_INT128__
    using i128 = __int128;
    using u128 = unsigned __int128;

#else
    #include "Debug.h"

    struct i128
    {
        SSSENGINE_NOT_IMPLEMENTED
    };

    struct u128
    {
        SSSENGINE_NOT_IMPLEMENTED
    };
#endif
} // namespace SSSEngine::Math
