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

#include <unordered_map>

#include "HelperMacros.h"
#include "Keycodes.h"
#include "ButtonState.h"
#include "Vector.h"

namespace SSSEngine::SSSInput
{
    // TODO: This is an initial input system and will be updated later
    // It will need a way to figure each event even during each frame by buffering input and dispatching events
    SSSENGINE_GLOBAL std::unordered_map<KeyboardCodes, ButtonState> KeyboardButtons;
    SSSENGINE_GLOBAL std::unordered_map<MouseButton, ButtonState> MouseButtons;
    SSSENGINE_GLOBAL SSSMath::Vector2<float> MouseDirection;
} // namespace SSSEngine::SSSInput
