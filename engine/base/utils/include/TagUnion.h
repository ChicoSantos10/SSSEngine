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

#include "Debug.h"
#include "Traits.h"
#include "Types.h"

namespace SSSEngine
{
    template<typename... Args>
        requires(sizeof...(Args) > 1)
    class TagUnion
    {
        public:
        static constexpr auto Alignment = MaxAlignmentValue<Args...>;
        static constexpr auto Size = MaxSizeValue<Args...>;

        private:
        alignas(Alignment) byte m_storage[Size];
        i8 m_tag = -1;
    };

    SSSENGINE_STATIC_ASSERT(alignof(TagUnion<u8, u16, u32, u64>) == 8, "");
    SSSENGINE_STATIC_ASSERT(sizeof(TagUnion<u8, u16, u32, u64>) == 16, "");
} // namespace SSSEngine
