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

/**
 * @file
 * @brief
 */

#include <Windows.h>
#include "Library.h"
#include "Types.h"

namespace SSSEngine::Platform
{
    void *LoadSharedLibrary(const wchar_t *path, const int flags)
    {
        return LoadLibraryEx(path, nullptr, flags);
    }

    void UnloadSharedLibrary(void *handle)
    {
        FreeLibrary(static_cast<HMODULE>(handle));
    }

    functionPtr GetFunctionAddressFromLibrary(void *handle, const char *funcName)
    {
        // INVESTIGATE: Consider exporting by ordinals using .def files instead to make the loading process faster
        //      https://learn.microsoft.com/en-us/cpp/build/exporting-from-a-dll-using-def-files?view=msvc-170
        return reinterpret_cast<functionPtr>(GetProcAddress(static_cast<HMODULE>(handle), funcName));
    }
} // namespace SSSEngine::Platform
