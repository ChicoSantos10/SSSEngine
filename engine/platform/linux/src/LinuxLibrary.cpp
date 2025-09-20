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

/*
 * @file
 * @brief Implementation of Linux Library functions
 *
 */

#include "Library.h"
#include "Debug.h"
#include "Types.h"

#include <dlfcn.h>

namespace SSSEngine::Platform
{
    LibraryHandle LoadSharedLibrary(const char *path, int flags)
    {
        return {dlopen(path, flags)};
    }

    void UnloadSharedLibrary(LibraryHandle libraryHandle)
    {
        dlclose(libraryHandle.handle);
        libraryHandle.handle = nullptr;
    }

    functionPtr GetFunctionAddressFromLibrary(LibraryHandle libraryHandle, const char *funcName)
    {
        SSSENGINE_ASSERT(libraryHandle.handle);

        return reinterpret_cast<functionPtr>(dlsym(libraryHandle.handle, funcName));
    }
} // namespace SSSEngine::Platform
