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

#include <cstdint>
#include "Attributes.h"
#include "Concepts.h"
#include <iostream>
#include <stdexcept>

namespace SSSEngine
{
    /* LOW_PRIORITY:
     * Have a small amount of memory for library handles that we can pass around instead of void*
     * Then we can have a struct Handle; and we also avoid leaking implementation details since consumers of the class
     * Wouldn't have access to anything inside. Also use RAII.
     * INVESTIGATE: Can we use protected memory? To prevent someone deleting it?
     * Under Windows look at: VirtualAlloc and VirtualProtect
     */
    void *LoadSharedLibrary(const wchar_t *path, int flags);
    void UnloadSharedLibrary(void *libraryHandle);
    void *GetFunctionAddressFromLibrary(void *libraryHandle, const char *funcName);

    // INVESTIGATE: Think about error handling. Should we throw an exception? Return an optional?
    template <Function T>
    SSSENGINE_FORCE_INLINE T LoadFunction(void *libraryHandle, const char *name)
    {
        T type = reinterpret_cast<T>(GetFunctionAddressFromLibrary(libraryHandle, name));
        if (!type)
            throw std::runtime_error("Failed to load function " + std::string(name));
        return type;
    }
} // namespace SSSEngine
