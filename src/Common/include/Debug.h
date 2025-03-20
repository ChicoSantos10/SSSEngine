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

/* INVESTIGATE: Have different types of assertions:
 *  -Debug: All assertions should be built
 *  -Internal: Same as debug?
 *  -ReleaseEditor: Keep assertions on API functions to ensure that the engine user does not make mistakes
 *      (example being on container access and stuff)
 *  -Release: No assertions
 *  An alternative would be to only include the SSSENGINE_ASSERTIONS define only on public API for Editor builds
 *  -----
 *   Is the __debugbreak useful?
 */
#ifdef SSSENGINE_ASSERTIONS

#define SSSENGINE_DEBUG_BREAK __debugbreak()
// NOTE: Seems like mingw implements __debugbreak as well
// #ifdef SSSENGINE_MSVC
// #else
// #ifdef __has_builtin
// #if __has_builtin(__debugbreak)
// #define SSSENGINE_DEBUG_BREAK __debugbreak()
// #elif __has_builtin(__builtin_debugtrap)
// #define SSSENGINE_DEBUG_BREAK __builtin_debugtrap()
// #elif __has_builtin(__builtin_trap)
// #define SSSENGINE_DEBUG_BREAK __builtin_trap()
// #endif
// #endif
// #endif

struct FrameContext;

// INVESTIGATE: Can we have this in a namespace?
void ReportAssertionFailure(const wchar_t *message, const wchar_t *file, unsigned line);

#include "HelperMacros.h"
/**
 * @brief Warning: This converts into [[assume(expression)]] when assertions are turned off. DO NOT PUT expressions that
 * have side effects!!!
 * @param expression The expression to be tested. Since we assert that it must not be true we can then tell the compiler
 * it can assume to be true potentially allowing for some optimizations
 */
#define SSSENGINE_ASSERT(expression)                                                                                   \
    (void)((!!(expression)) ||                                                                                         \
           (ReportAssertionFailure(SSSENGINE_WIDE_STRING(expression), SSSENGINE_WIDE(__FILE__), (unsigned)(__LINE__)), \
            SSSENGINE_DEBUG_BREAK,                                                                                     \
            0))

#define SSSENGINE_UNREACHABLE SSSENGINE_ASSERT(false && "Supposedly unreachable code reached")
#else
#include "Attributes.h"
#define SSSENGINE_ASSERT(expression) SSSENGINE_ASSUME(expression)
#ifdef SSSENGINE_MSVC
#define SSSENGINE_UNREACHABLE __assume(0)
#elif SSSENGINE_MINGW
#define SSSENGINE_UNREACHABLE __builtin_unreachable()
#endif
#endif

#define SSSENGINE_STATIC_ASSERT(expression) static_assert(expression);
