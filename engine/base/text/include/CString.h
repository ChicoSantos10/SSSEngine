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
 * @brief C String Helper Functions
 */

#pragma once

#include "Attributes.h"
#include "Encoding.h"

namespace SSSEngine::Text
{
    template<EncodingConcept Encoding>
        SSSENGINE_PURE
    bool StringEqual(const typename Encoding::CodeUnitType *lhs, const typename Encoding::CodeUnitType *rhs) noexcept
    {
        if(lhs == rhs)
        {
            return true;
        }

        while(*lhs == *rhs)
        {
            if(!*lhs)
            {
                return true;
            }

            ++lhs, ++rhs;
        }

        return false;
    }
} // namespace SSSEngine::Text
