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

namespace SSSEngine
{
    // NOTE: In case needed this checks to see if all parameters are the same type
    // INVESTIGATE: Can we make a concept for this?
    // requires std::conjunction_v<std::is_same<T, Bits>...>

    // TODO: Change to sssengine concepts
    template <std::integral T, std::integral... Bits> requires std::conjunction_v<std::is_same<T, Bits>...>
    SSSENGINE_FORCE_INLINE constexpr auto Join(T first, Bits... bits) -> T
    {
        return first | (bits | ...);
    }

    template <std::integral T>
    SSSENGINE_FORCE_INLINE constexpr bool HasBits(T first, T second)
    {
        return first & second;
    }

    template <std::integral T>
    SSSENGINE_FORCE_INLINE constexpr T WithoutBits(T first, T second)
    {
        return first & ~second;
    }
}
