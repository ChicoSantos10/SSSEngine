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
// TODO: Implement the ReportAssertion and finish the ASSERT Macro
//

// TODO: Move this somewhere else
#define SSSENGINE_WIDE_STRING_(x) L ## x
#define SSSENGINE_WIDE_STRING(x) SSSENGINE_WIDE_STRING_(x)

#ifdef SSSENGINE_ASSERTIONS

void ReportAssertionFailure(const wchar_t *message, const wchar_t *file, unsigned line);

#define SSSENGINE_ASSERT(expression) (void)(                                                       \
            (!!(expression)) ||                                                          \
            (ReportAssertionFailure(SSSENGINE_WIDE_STRING(#expression), SSSENGINE_WIDE_STRING(__FILE__), (unsigned)(__LINE__)), \
            __debugbreak(), \
            0)  \
        )

#else
#define SSSENGINE_ASSERT(expression)
#endif
