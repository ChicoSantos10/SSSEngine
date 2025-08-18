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

#include "windows.h"
#include <xinput.h>

#include "Input.h"

namespace SSSEngine::Input
{
    // TODO: This function currently returns whether the application should keep running. Later we should listen to
    // window closed events and end application if the main window closes.
    bool HandleInput()
    {
        MSG msg = {};
        while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // TODO: This should only handle input. Leave window events outside
            if(msg.message == WM_QUIT)
            {
                return false;
            }
            if(msg.message == WM_DESTROY)
            {
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Gamepad
        GamepadInput();

        return true;
    }

    // TODO: Finish GamepadInput
    void GamepadInput()
    {
        for(int i = 0; i < XUSER_MAX_COUNT; ++i)
        {
            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));

            if(const DWORD dwResult = XInputGetState(i, &state); dwResult == ERROR_SUCCESS)
            {
                // Controller is connected
                const auto [wButtons, bLeftTrigger, bRightTrigger, sThumbLX, sThumbLY, sThumbRX, sThumbRY] = state.Gamepad;

                if(wButtons & XINPUT_GAMEPAD_A)
                {
                    // Vibrations
                    XINPUT_VIBRATION vibration;
                    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
                    vibration.wLeftMotorSpeed = 1000;
                    vibration.wRightMotorSpeed = 2000;
                    XInputSetState(i, &vibration);
                }
                else
                {
                    // Stop vibrations
                    XINPUT_VIBRATION vibration;
                    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
                    XInputSetState(i, &vibration);
                }

                const float leftThumbX = sThumbLX;
                const float leftThumbY = sThumbLY;

                float magnitude = sqrtf(leftThumbX * leftThumbX + leftThumbY * leftThumbY);

                // Direction
                float normalizedLx = leftThumbX / magnitude;
                float normalizedLy = leftThumbY / magnitude;

                float normalizedMagnitude = 0;
                if(magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
                {
                    // clip the magnitude at its expected maximum value
                    if(magnitude > 32767)
                        magnitude = 32767;

                    magnitude -= XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

                    normalizedMagnitude = magnitude / (32767 - XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                }
                else
                {
                    magnitude = 0.0;
                    normalizedMagnitude = 0.0;
                }
            }
            else
            {
                // Controller is not connected
            }
        }
    }
} // namespace SSSEngine::Input
