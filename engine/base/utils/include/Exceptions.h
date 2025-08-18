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
    SSSENGINE_PURE const char *what() const noexcept override
    {
        return Reason;
    }
};

#endif
