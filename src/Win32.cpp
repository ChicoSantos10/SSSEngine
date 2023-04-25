//
// Created by Francisco Santos on 22/04/2023.
//

#include <windows.h>
#include <cstdio>
#include <iostream>
#include <xinput.h>
#include <cmath>
#include <xaudio2.h>
#include <wrl\client.h>

LRESULT CALLBACK WindowMessageCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InitConsole();
void CloseConsole();
void GamepadInput();

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	InitConsole();

	const WCHAR CLASS_NAME[] = L"SSS Engine";

	WNDCLASSEXW wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowMessageCallback;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	if (RegisterClassExW(&wc) == 0)
	{
		OutputDebugStringW(L"Window class creation failed");
		return -1;
	}

	HWND hwnd = CreateWindowExW(
		0,
		CLASS_NAME,
		L"SSS Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (hwnd == nullptr)
	{
		OutputDebugStringW(L"Window creation failed");
		return -1;
	}

	// COM initialization
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"COM initialization failed");
		return -1;
	}

	// XAudio2
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 initialization failed");
		return -1;
	}

	IXAudio2MasteringVoice* pMasteringVoice;
	hr = xAudio2->CreateMasteringVoice(&pMasteringVoice);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 mastering voice initialization failed");
		return -1;
	}

	WAVEFORMATEXTENSIBLE wave = { };
	wave.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	wave.Format.nChannels = 2;
	wave.Format.nSamplesPerSec = 48000;
	wave.Format.wBitsPerSample = 16;
	wave.Format.nBlockAlign = (wave.Format.nChannels * wave.Format.wBitsPerSample) / 8;
	wave.Format.nAvgBytesPerSec = wave.Format.nSamplesPerSec * wave.Format.nBlockAlign;
	wave.Format.cbSize = sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
	wave.Samples.wValidBitsPerSample = wave.Format.wBitsPerSample;
	wave.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
	wave.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;

	XAUDIO2_BUFFER buffer = { };
	buffer.AudioBytes = wave.Format.nSamplesPerSec * wave.Format.nChannels * (wave.Format.wBitsPerSample / 8);
	buffer.pAudioData = new BYTE[buffer.AudioBytes];
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	// Populate the audio buffer with some simple sine wave data at 440 Hz.
	for (DWORD i = 0; i < wave.Format.nSamplesPerSec; ++i)
	{
		float t = i / (float)wave.Format.nSamplesPerSec;
		constexpr float TwoPi = 2.0f * 3.14159265358979323846264338327950288419716939937510f;

		// Write the sample to the buffer
		((short*)buffer.pAudioData)[i * 2 + 0] = (short)(sinf(t*TwoPi*440.0f) * 30000.0f); // Left channel
		((short*)buffer.pAudioData)[i * 2 + 1] = (short)(sinf(t*TwoPi*440.0f) * 30000.0f); // Right channel
	}

	IXAudio2SourceVoice* pSourceVoice;
	hr = xAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wave);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 source voice initialization failed");
		return -1;
	}

	hr = pSourceVoice->SubmitSourceBuffer(&buffer);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 source buffer initialization failed");
		return -1;
	}

	hr = pSourceVoice->Start(0);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 source voice start failed");
		return -1;
	}
	pSourceVoice->SetVolume(1);

	ShowWindow(hwnd, nShowCmd);
	bool isRunning = true;
	while (isRunning)
	{
		MSG msg = { };
		while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		// Gamepad
		GamepadInput();
	}

	CloseConsole();
	return 0;
}

LRESULT CALLBACK WindowMessageCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_MENUHILIGHT));

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_LBUTTONDOWN:
		{
			std::cout << "Left mouse button pressed" << std::endl;
			return 0;
		}
		default:
			return DefWindowProcW(hwnd, msg, wParam, lParam);
	}
}

void GamepadInput() {
	DWORD dwResult;
	for (int i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		dwResult = XInputGetState(i, &state);
		if (dwResult == ERROR_SUCCESS)
		{
			// Controller is connected
			XINPUT_GAMEPAD gamepad = state.Gamepad;

			if (gamepad.wButtons & XINPUT_GAMEPAD_A)
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

			float leftThumbX = gamepad.sThumbLX;
			float leftThumbY = gamepad.sThumbLY;

			float magnitude = sqrtf(leftThumbX * leftThumbX + leftThumbY * leftThumbY);

			// Direction
			float normalizedLX = leftThumbX / magnitude;
			float normalizedLY = leftThumbY / magnitude;

			float normalizedMagnitude = 0;
			if (magnitude > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
			{
				//clip the magnitude at its expected maximum value
				if (magnitude > 32767)
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

void InitConsole()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);
}

void CloseConsole()
{
	FreeConsole();
}