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

// TODO: Remove this include and fix all errors
#include <cassert>
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
// TODO: Move this somewhere else
#define SSSENGINE_WIDE_STRING_(x) L ## x
#define SSSENGINE_WIDE_STRING(x) SSSENGINE_WIDE_STRING_(x)

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

// INVESTIGATE: Can we have this in a namespace?
void ReportAssertionFailure(const wchar_t *message, const wchar_t *file, unsigned line);

#define SSSENGINE_ASSERT(expression) (void)(                                                       \
            (!!(expression)) ||                                                          \
            (ReportAssertionFailure(SSSENGINE_WIDE_STRING(#expression), SSSENGINE_WIDE_STRING(__FILE__), (unsigned)(__LINE__)), \
            SSSENGINE_DEBUG_BREAK, \
            0)  \
        )
#else
// TODO: Replace Assert to be Assume.
#define SSSENGINE_ASSERT(expression)
#endif
