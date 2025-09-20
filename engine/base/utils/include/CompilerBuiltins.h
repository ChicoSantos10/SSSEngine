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
 * @brief File with macro defines for compiler builtin functions
 */

#pragma once

#include "Attributes.h"
#include "Concepts.h"

namespace SSSEngine::INTERNAL
{
#ifdef SSSENGINE_MSVC
#elif SSSENGINE_CLANG

    #if __has_builtin(__builtin_ctzg)
    template<IntegralConcept T>
        requires(IsUnsigned<T>)
    SSSENGINE_FORCE_INLINE constexpr int BuiltinCtz(T mask)
    {
        return __builtin_ctzg(mask);
    }
    #else
    template<IntegralConcept T>
        requires(IsUnsigned<T>)
    SSSENGINE_FORCE_INLINE constexpr int BuiltinCtz(T mask)
    {
        using Type = decltype(mask);

        if constexpr(SameType<Type, unsigned int>)
            return __builtin_ctz(mask);

        if constexpr(SameType<Type, unsigned long>)
            return __builtin_ctzl(mask);

        return __builtin_ctzll(mask);
    }
    #endif
    #if __has_builtin(__builtin_clzg)
    template<IntegralConcept T>
        requires(IsUnsigned<T>)
    SSSENGINE_FORCE_INLINE constexpr int BuiltinClz(T mask)
    {
        return __builtin_clzg(mask);
    }
    #else
    #endif

    #define SSSENGINE_BITCOPY(to, from) (__builtin_bit_cast(to, from))
#endif
} // namespace SSSEngine::INTERNAL
