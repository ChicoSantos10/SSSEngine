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

#include "StringView.h"
#include "Types.h"
#include "Attributes.h"
#include "Concepts.h"

namespace SSSEngine::Platform
{
    struct LibraryHandle
    {
        void *handle;

        explicit operator bool() const
        {
            return handle;
        }
    };

    /* LOW_PRIORITY:
     * Have a small amount of memory for library handles that we can pass around instead of void*
     * Then we can have a struct Handle; and we also avoid leaking implementation details since consumers of the class
     * Wouldn't have access to anything inside. Also use RAII.
     * INVESTIGATE: Can we use protected memory? To prevent someone deleting it?
     * Under Windows look at: VirtualAlloc and VirtualProtect
     * Also the wchar_t* is not gonna be portable since linux will probably be a char*
     */
    /**
     * @brief Loads a shared library dynamically
     *
     * @param path The relative path to a library
     * @param flags [TODO:parameter]
     */
    LibraryHandle LoadSharedLibrary(const char *path, int flags);

    /**
     * @brief Unloads a shared library loaded by @see LoadSharedLibrary(path, flags)
     *
     * @param libraryHandle The handle returned by LoadSharedLibrary(path, flags)
     */
    void UnloadSharedLibrary(LibraryHandle libraryHandle);

    /**
     * @brief Loads a function from a library loaded by @see LoadSharedLibrary(path, flags)
     *
     * @param libraryHandle The handle returned by LoadSharedLibrary(path, flags)
     * @param funcName The name of the function to load
     * @return A pointer to the function loaded
     */
    functionPtr GetFunctionAddressFromLibrary(LibraryHandle libraryHandle, Text::Utf8View funcName);

    // INVESTIGATE: Think about error handling. Should we throw an exception? Return an optional?
    /**
     * @brief Loads a function and casts it to the proper type
     *
     * @tparam A function pointer Type
     * @param libraryHandle The library handle returned by @see LoadSharedLibrary(path, flags)
     * @param name The name of the function
     * @return The function casted to the appropriate type
     */
    template<FunctionPointerConcept T>
    SSSENGINE_FORCE_INLINE
    T LoadFunction(LibraryHandle libraryHandle, Text::Utf8View name)
    {
        T type = reinterpret_cast<T>(GetFunctionAddressFromLibrary(libraryHandle, name));
        if(!type)
        {
            // TODO: Handle Error
        }
        return type;
    }
} // namespace SSSEngine::Platform
