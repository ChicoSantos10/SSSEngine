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
 * @brief ASCII Encoding
 */

#pragma once

#include "Attributes.h"
#include "Debug.h"
#include "Encoding.h"

namespace SSSEngine::Text
{
    struct AsciiEncoding
    {
        using CodeUnitType = char;
        using CodePointType = char;

        SSSENGINE_FORCE_INLINE
        constexpr static u8 CharLength(CodeUnitType)
        {
            return 1;
        }

        SSSENGINE_FORCE_INLINE
        constexpr static bool ValidCodepoint(const CodeUnitType *, CodeUnitType)
        {
            return true;
        }

        SSSENGINE_FORCE_INLINE
        constexpr static bool ValidString(const CodeUnitType *, SizeType)
        {
            return true;
        }

        SSSENGINE_FORCE_INLINE
        constexpr static CodePointType Decode(const CodeUnitType *data, SizeType)
        {
            return data[0];
        }
    };

    SSSENGINE_STATIC_ASSERT(EncodingConcept<AsciiEncoding>, "ASCII must be a type of encoding");
} // namespace SSSEngine::Text
