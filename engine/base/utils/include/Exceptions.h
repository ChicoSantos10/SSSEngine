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

#include <exception>
#include "Attributes.h"
#if defined(SSSENGINE_DEBUG) || defined(SSSENGINE_INTERNAL)

/**
 * @class NotImplementedException
 * @brief Represents code that was not implemented. Only works in builds with debug capabilities.
 *
 */
class NotImplementedException final : public std::exception
{
    static constexpr auto Reason = "Not implemented";

    public:
    SSSENGINE_PURE const char *what() const noexcept override { return Reason; }
};

#endif
