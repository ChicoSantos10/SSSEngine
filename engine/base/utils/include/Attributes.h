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

#include "HelperMacros.h"

// Exports and imports
#define SSSENGINE_DLL_EXPORT extern "C" [[maybe_unused]] __declspec(dllexport)
#define SSSENGINE_DLL_IMPORT __declspec(dllimport)

#pragma region Inlining

#ifdef SSSENGINE_ALLOW_FORCE_INLINE
    #ifdef SSSENGINE_MSVC
        #define SSSENGINE_FORCE_INLINE __forceinline
    #elif SSSENGINE_MINGW
        #define SSSENGINE_FORCE_INLINE [[using gnu: always_inline, inline]] inline
    #endif
#else
    #define SSSENGINE_FORCE_INLINE inline
#endif

#ifdef SSSENGINE_ALLOW_NO_INLINE
    #ifdef SSSENGINE_MSVC
        #define SSSENGINE_NO_INLINE declspec(noinline)
    #elif SSSENGINE_MINGW
        #define SSSENGINE_NO_INLINE __attribute__((noinline))
    #endif
#else
    #define SSSENGINE_NO_INLINE
#endif

// NOTE: As fas as I know there is no support for it from MSVC or GCC. Will keep it in case we ever get this option
#ifdef SSSENGINE_ALLOW_HINT_INLINE_STATEMENT
    #ifdef SSSENGINE_MSVC
        #define SSSENGINE_HINT_INLINE_STATEMENT
    #elif SSSENGINE_MINGW
        #define SSSENGINE_HINT_INLINE_STATEMENT
    #endif
    #ifdef SSSENGINE_MSVC
        #define SSSENGINE_HINT_INLINE_STATEMENT_RECURSIVE
    #elif SSSENGINE_MINGW
        #define SSSENGINE_HINT_INLINE_STATEMENT_RECURSIVE
    #endif
#else
    #define SSSENGINE_HINT_INLINE_STATEMENT
    #define SSSENGINE_HINT_INLINE_STATEMENT_RECURSIVE
#endif

#ifdef SSSENGINE_ALLOW_FORCE_INLINE_STATEMENT
    #ifdef SSSENGINE_MSVC
        // NOTE: As fas as I know there is no support for it from MSVC
        #define SSSENGINE_FORCE_INLINE_STATEMENT
    #elif SSSENGINE_MINGW
        #define SSSENGINE_FORCE_INLINE_STATEMENT SSSENGINE_PRAGMA(FORCEINLINE)
    #endif
    #ifdef SSSENGINE_MSVC
        // NOTE: As fas as I know there is no support for it from MSVC
        #define SSSENGINE_FORCE_INLINE_STATEMENT_RECURSIVE
    #elif SSSENGINE_MINGW
        #define SSSENGINE_FORCE_INLINE_STATEMENT_RECURSIVE SSSENGINE_PRAGMA(FORCEINLINE_RECURSIVE)
    #endif
#else
    #define SSSENGINE_FORCE_INLINE_STATEMENT
    #define SSSENGINE_FORCE_INLINE_STATEMENT_RECURSIVE
#endif

#ifdef SSSENGINE_ALLOW_NO_INLINE_STATEMENT
    #ifdef SSSENGINE_MSVC
        // NOTE: As fas as I know there is no support for it from MSVC
        #define SSSENGINE_NO_INLINE_STATEMENT
    #elif SSSENGINE_MINGW
        #define SSSENGINE_NO_INLINE_STATEMENT SSSENGINE_PRAGMA(NOINLINE)
    #endif
#else
    #define SSSENGINE_NO_INLINE_STATEMENT
#endif

#pragma endregion

#pragma region Attributes

#define SSSENGINE_PURE [[nodiscard]]
#define SSSENGINE_NO_RETURN [[noreturn]]
#define SSSENGINE_FALLTHROUGH [[fallthrough]]
#define SSSENGINE_DEPRECATED(reason) [[deprecated(#reason)]]
#define SSSENGINE_CARRIES_DEPENDENCY [[carries_dependency]]
#define SSSENGINE_LIKELY [[likely]]
#define SSSENGINE_UNLIKELY [[unlikely]]
#define SSSENGINE_NO_UNIQUE_ADDRESS [[no_unique_address]]
#ifdef SSSENGINE_MSVC
    #define SSSENGINE_ASSUME(expression) __assume(expression)
#elif SSSENGINE_MINGW
    #define SSSENGINE_ASSUME(expression) [[assume(expression)]]
#endif
#define SSSENGINE_INDETERMINATE [[indeterminate]]
// #define SSSENGINE_OPTIMIZE_FOR_SYNCHRONIZED [[optimize_for_synchronized]]
#define SSSENGINE_MAYBE_UNUSED [[maybe_unused]]

#pragma endregion

// INVESTIGATE: Pragmas
#define SSSENGINE_PACK_PUSH(command) SSSENGINE_PRAGMA(pack(push, command))
#define SSSENGINE_PACK_SHOW SSSENGINE_PRAGMA(pack(show))
#define SSSENGINE_PACK_POP SSSENGINE_PRAGMA(pack(pop))

// LOW_PRIORITY: assume_aligned -> https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1007r3.pdf

// INVESTIGATE: Check gnu and msvc attributes
// GCC: https://gcc.gnu.org/onlinedocs/gcc/Function-Attributes.html
