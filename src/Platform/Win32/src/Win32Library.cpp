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

#include <cstddef>
#include <concepts>
#include <functional>
#include <string>

// Declaration of the concept “Hashable”, which is satisfied by any type “T”
// such that for values “a” of type “T”, the expression std::hash<T>{}(a)
// compiles and its result is convertible to std::size_t
template<typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

#include <Windows.h>
#include "Library.h"

namespace SSSEngine
{
    void *LoadSharedLibrary(const wchar_t *path, const int flags) { return LoadLibraryEx(path, nullptr, flags); }

    void UnloadSharedLibrary(void *handle) { FreeLibrary(static_cast<HMODULE>(handle)); }

    void *GetFunctionAddressFromLibrary(void *handle, const char *funcName)
    {
        // INVESTIGATE: Consider exporting by ordinals using .def files instead to make the loading process faster
        //      https://learn.microsoft.com/en-us/cpp/build/exporting-from-a-dll-using-def-files?view=msvc-170
        return GetProcAddress(static_cast<HMODULE>(handle), funcName);
    }
} // namespace SSSEngine
