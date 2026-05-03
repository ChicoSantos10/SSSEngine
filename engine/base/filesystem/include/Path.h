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
 * @brief Represents a path
 */

#include "Attributes.h"
#include "StringView.h"
#include "String.h"

namespace SSSEngine::FileSystem
{
    SSSENGINE_FORCE_INLINE
    Text::Utf8 NormalizePath(Text::Utf8View string)
    {
        return {};
    }

    /**
     * @class Path
     * @brief Represents a path to a directory or a file
     *
     */
    class Path
    {
    };

    /**
     * @class RawPath
     * @brief Represents a compile time path
     *
     */
    class RawPath
    {
      public:
        consteval explicit RawPath(Text::Utf8View path) : m_path{NormalizePath(path)} {};

      private:
        Text::Utf8 m_path;
    };
} // namespace SSSEngine::FileSystem
