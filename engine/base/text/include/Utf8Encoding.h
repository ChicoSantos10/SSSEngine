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
 * @brief Represents the UTF-8 String Encoding
 */

#pragma once

#include "Attributes.h"
#include "Encoding.h"
#include "Debug.h"
#include "HelperMacros.h"
#include "Types.h"

namespace SSSEngine::Text
{
    struct Utf8Encoding
    {
        using CodeUnitType = char8;
        using CodePointType = char32;

        /**
         * @brief Calculates how big the code point
         *
         * @param codeUnit The header of the code point
         * @return The number of code units, this code point contains
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr static u8 CharLength(CodeUnitType codeUnit) noexcept
        {
            if((codeUnit & 0x80) == 0)
                return 1;
            if((codeUnit & 0xE0) == 0xC0)
                return 2;
            if((codeUnit & 0xF0) == 0xE0)
                return 3;
            if((codeUnit & 0xF8) == 0xF0)
                return 4;

            return 0;
        }

        /**
         * @brief Checks if it's a valid code point.
         *
         * @important This function assumes that if size is 1 then the header must be valid and therefore the code point
         * is also valid
         *
         * @param data A pointer to the first byte to check.
         * @param size The size of the code point.
         * @return True if all bytes are continuation bytes, false otherwise
         */
        SSSENGINE_PURE SSSENGINE_GLOBAL
        constexpr static bool ValidCodepoint(const CodeUnitType *data, u8 size) noexcept
        {
            if(size == 0)
                return false;

            if(size == 1)
                return true;

            for(char8 i = 1; i < size; ++i)
            {
                char8 b = data[i];
                if(!IsContinuationByte(b))
                    return false;
            }

            return true;
        }

        /**
         * @brief Checks if a given string is valid UTF-8
         *
         * @param data A ponter to the string
         * @param size The size of the string
         * @return True if the string is valid UTF-8, false otherwise
         */
        SSSENGINE_PURE SSSENGINE_GLOBAL
        constexpr static bool ValidString(const CodeUnitType *data, SizeType size) noexcept
        {
            // TODO: This does not need to be here. Instead should be in iterators or string free functions
            for(SizeType i = 0; i < size;)
            {
                u8 byte = data[i];
                auto codePointSize = CharLength(byte);
                if(!ValidCodepoint(&data[i], codePointSize))
                    return false;

                i += codePointSize;
            }

            return true;
        }

        /**
         * @brief Decodes a series of code units into a code point
         *
         * @param data A pointer to the first byte of the code point
         * @param size The size of the code point. @see CharLength
         * @return A code point
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr static CodePointType Decode(const CodeUnitType *data, SizeType size) noexcept
        {
            switch(size)
            {
                case 1:
                    return CodePointType(data[0]);
                case 2:
                    return ((data[0] & 0x1F) << 6) | (data[1] & 0x3F);
                case 3:
                    return ((data[0] & 0x0F) << 12) | ((data[1] & 0x3F) << 6) | (data[2] & 0x3F);
                case 4:
                    return ((data[0] & 0x07) << 18) | ((data[1] & 0x3F) << 12) | ((data[2] & 0x3F) << 6) | (data[3] & 0x3F);
                default:
                    SSSENGINE_UNREACHABLE;
            }
        }

        /**
         * @brief Checks to see if byte is a valid continuation byte
         *
         * In Unicode a continuation byte must have 10 as the first 2 bits when counting from the most significant bit
         *
         * @param byte The byte to check
         * @return True if the byte is a continuation byte, false otherwise
         */
        SSSENGINE_PURE SSSENGINE_FORCE_INLINE
        constexpr static bool IsContinuationByte(CodeUnitType byte) noexcept
        {
            return (byte & 0xC0) == 0x80;
        }
    };

    SSSENGINE_STATIC_ASSERT(EncodingConcept<Utf8Encoding>, "UTF-8 must be an encoding concept");
} // namespace SSSEngine::Text
