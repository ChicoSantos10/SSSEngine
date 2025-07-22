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

#include "Types.h"

namespace SSSEngine::SSSInput
{
    enum class KeyboardCodes : u8
    {
        Number1,
        Number2,
        Number3,
        Number4,
        Number5,
        Number6,
        Number7,
        Number8,
        Number9,
        Number0,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        X,
        W,
        Y,
        Z,
        Backspace,
        Escape,
        Tab,
        Clear,
        Enter,
        Caps,
        Space,
        PageUp,
        PageDown,
        End,
        Home,
        Print,
        Insert,
        Delete,
        Help,
        LeftMeta,
        RightMeta,
        Numpad0,
        Numpad1,
        Numpad2,
        Numpad3,
        Numpad4,
        Numpad5,
        Numpad6,
        Numpad7,
        Numpad8,
        Numpad9,
        Asterisk,
        Add,
        Subtract,
        NumLock,
        Divide,
        Dot,
        Comma,
        ScrollLock,
        LShift,
        RShift,
        LControl,
        RControl,
        LAlt,
        RAlt,
        VolumeUp,
        VolumeDown,
        VolumeMute,
        ArrowUp,
        ArrowDown,
        ArrowLeft,
        ArrowRight,
    };

    enum class MouseButton : u8
    {
        Left = 1,
        Right = 0x02,
        Middle = 0x04,
        Scroll = 0x08,
        Btn1 = 0x10,
        Btn2 = 0x20,
    };
} // namespace SSSEngine::SSSInput
