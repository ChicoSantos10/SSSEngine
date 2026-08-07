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

#define SSSENGINE_STRING(x) #x

#define SSSENGINE_WIDE_STRING_(x) L##x
#define SSSENGINE_WIDE_STRING(x) SSSENGINE_WIDE_STRING_(#x)
#define SSSENGINE_WIDE(x) SSSENGINE_WIDE_STRING_(x)

#define SSSENGINE_UTF8(message) u8##message
#define SSSENGINE_UTF16(message) u##message
#define SSSENGINE_UTF32(message) U##message
#define SSSENGINE_ASCII(message) message

#define SSSENGINE_UTF8_FILE_(x) SSSENGINE_UTF8(x)
#define SSSENGINE_UTF8_FILE SSSENGINE_UTF8_FILE_(__FILE__)
#define SSSENGINE_UTF8_STRING(x) SSSENGINE_UTF8(#x)

#define SSSENGINE_TEXT(x) SSSENGINE_UTF8(x)
#define SSSENGINE_TEXT_STRING(x) SSSENGINE_UTF8(#x)
#define SSSENGINE_FILE SSSENGINE_UTF8_FILE
#define SSSENGINE_LINE __LINE__

#define SSSENGINE_PRAGMA(x) _Pragma(#x)

#define SSSENGINE_GLOBAL inline
#define SSSENGINE_INTERNAL static
#define SSSENGINE_FUNCTION_LOCAL static

#define SSSENGINE_DISCARD (void)

#define SSSENGINE_LIB(library) SSSENGINE_PRAGMA(comment(lib, #library))

#ifdef SSSENGINE_MSVC
#elif SSSENGINE_GCC || SSSENGINE_CLANG
    #define SSSENGINE_PACKED(name) struct __attribute__((packed)) name
#endif
