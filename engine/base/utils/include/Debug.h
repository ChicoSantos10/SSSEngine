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

/* INVESTIGATE: Have different types of assertions:
 *  -Debug: All assertions should be built
 *  -Internal: Same as debug?
 *  -ReleaseEditor: Keep assertions on API functions to ensure that the engine user does not make mistakes
 *      (example being on container access and stuff)
 *  -Release: No assertions
 *  An alternative would be to only include the SSSENGINE_ASSERTIONS define only on public API for Editor builds
 */
#ifdef SSSENGINE_ASSERTIONS

    #define SSSENGINE_DEBUG_BREAK __debugbreak()

// INVESTIGATE: Can we have this in a namespace?
/**
 * @brief Sets a breakpoint while logging the error
 *
 * @param message The error message
 * @param file The file that threw the error
 * @param line The line where the error occurred
 */
void ReportAssertionFailure(const wchar_t *message, const wchar_t *file, unsigned line);

    #include "HelperMacros.h"
    /**
     * @brief Warning: This converts into [[assume(expression)]] when assertions are turned off. DO NOT PUT expressions
     * that have side effects!!!
     * @param expression The expression to be tested. Since we assert that it must not be true we can then tell the
     * compiler it can assume to be true, potentially allowing for some optimizations
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

#define SSSENGINE_STATIC_ASSERT(expression, message) static_assert(expression, message);
