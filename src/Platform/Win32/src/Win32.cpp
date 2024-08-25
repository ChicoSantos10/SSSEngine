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

#include "windows.h"
#include <cstdio>
#include <xinput.h>
#include <cmath>
#include <wrl/client.h>
#include "../../../Common/include/Application.h"
#include "../../../../include/XAudioRedist/xaudio2.h"
#include "../../../../include/XAudioRedist/xaudio2fx.h"
#include "../../../../include/XAudioRedist/xapofx.h"
#include "../../../../include/XAudioRedist/xapo.h"
#include "Win32Window.h"
#include "../../../Renderer/include/Renderer.h"

void InitConsole();
void CloseConsole();
void GamepadInput();

// NOTE: For some reason this needs to be on main
extern "C" {
// ReSharper disable CppInconsistentNaming
__declspec(dllexport) extern const UINT D3D12SDKVersion = 610;
__declspec(dllexport) extern const char8_t *D3D12SDKPath = u8".\\Directx12\\D3D12\\";
// ReSharper restore CppInconsistentNaming
}

constexpr WCHAR WindowClassName[] = L"SSS Engine";

// TODO: Check for cpu attributes to ensure minimum specs
//  -> SIMD
//  -> Minimum memory perhaps or just try to allocate and if fails allocate less?
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	InitConsole();

	SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	/*SSSEngine::Application app;
	app.Run();*/

	SSSWin32::windowClass.cbSize = sizeof(WNDCLASSEX);
	SSSWin32::windowClass.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	SSSWin32::windowClass.lpfnWndProc = SSSWin32::MainWindowProcedure;
	SSSWin32::windowClass.hInstance = hInstance;
	SSSWin32::windowClass.lpszClassName = WindowClassName;
	SSSWin32::windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	SSSWin32::windowClass.lpszMenuName = RT_MENU;

	if (RegisterClassExW(&SSSWin32::windowClass) == 0)
	{
		OutputDebugStringW(L"Window class creation failed");
		return -1;
	}
	// COM initialization
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"COM initialization failed");
		return -1;
	}

	//Win32::Win32Window subWindow(640, 360, "Sub Window 1", wc, hwnd);
	//Win32::Win32Window subWindow2(640, 360, "Sub Window 2", wc, hwnd);

	//GetWindowRect(hwnd, &SSSEngineRenderer::DirectX::windowRect);
	//SSSEngineRenderer::DirectX::InitializeDirectx12(hwnd);
	SSSRenderer::LoadDirectx();
	SSSRenderer::LoadAssetsTest();

	SSSEngine::Window window(CW_USEDEFAULT, CW_USEDEFAULT, 1260, 720, SSSEngine::MainWindowName);

	//SSSEngineRenderer::Directx::SSSEngineRenderer renderer;
	//renderer.Initialize(hwnd);

	// XAudio2
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 initialization failed");
		return -1;
	}

	IXAudio2MasteringVoice *pMasteringVoice;
	hr = xAudio2->CreateMasteringVoice(&pMasteringVoice);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 mastering voice initialization failed");
		return -1;
	}

	WAVEFORMATEXTENSIBLE wave = {};
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

	XAUDIO2_BUFFER buffer = {};
	buffer.AudioBytes = wave.Format.nSamplesPerSec * wave.Format.nChannels * (wave.Format.wBitsPerSample / 8);
	buffer.pAudioData = new BYTE[buffer.AudioBytes];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	// Populate the audio buffer with some simple sine wave data at 440 Hz.
	for (DWORD i = 0; i < wave.Format.nSamplesPerSec; ++i)
	{
		float t = i / static_cast<float>(wave.Format.nSamplesPerSec);
		constexpr float twoPi = 2.0f * 3.14159265358979323846264338327950288419716939937510f;

		// Write the sample to the buffer
		((BYTE*)buffer.pAudioData)[i * 2 + 0] = static_cast<BYTE>(sinf(t * twoPi * 440.0f) * 30000.0f); // Left channel
		((BYTE*)buffer.pAudioData)[i * 2 + 1] = static_cast<BYTE>(sinf(t * twoPi * 440.0f) * 30000.0f); // Right channel
	}

	IXAudio2SubmixVoice *pSubMixVoice;
	hr = xAudio2->CreateSubmixVoice(&pSubMixVoice, 2, wave.Format.nSamplesPerSec, 0, 0, nullptr, nullptr);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 submix voice initialization failed");
		return -1;
	}

	XAUDIO2_SEND_DESCRIPTOR sendDescriptors = {0, pSubMixVoice};
	XAUDIO2_VOICE_SENDS sendList = {1, &sendDescriptors};

	IUnknown *reverbFx;
	if (FAILED(hr = XAudio2CreateReverb(&reverbFx)))
	{
		OutputDebugStringW(L"XAudio2 reverb initialization failed");
		return -1;
	}

	XAUDIO2_EFFECT_DESCRIPTOR descriptor = {reverbFx, true, 1};
	XAUDIO2_EFFECT_CHAIN effectChain = {1, &descriptor};
	XAUDIO2FX_REVERB_PARAMETERS reverbParameters;
	reverbParameters.ReflectionsDelay = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY;
	reverbParameters.ReverbDelay = XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY;
	reverbParameters.RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY;
	reverbParameters.PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	reverbParameters.PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION;
	reverbParameters.PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	reverbParameters.PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX;
	reverbParameters.EarlyDiffusion = XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION;
	reverbParameters.LateDiffusion = XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION;
	reverbParameters.LowEQGain = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN;
	reverbParameters.LowEQCutoff = XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF;
	reverbParameters.HighEQGain = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN;
	reverbParameters.HighEQCutoff = XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF;
	reverbParameters.RoomFilterFreq = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ;
	reverbParameters.RoomFilterMain = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN;
	reverbParameters.RoomFilterHF = XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF;
	reverbParameters.ReflectionsGain = XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN;
	reverbParameters.ReverbGain = XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN;
	reverbParameters.DecayTime = XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME;
	reverbParameters.Density = XAUDIO2FX_REVERB_DEFAULT_DENSITY;
	reverbParameters.RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE;
	reverbParameters.WetDryMix = XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX;

	IUnknown *echoFx;
	CreateFX(CLSID_FXEcho, &echoFx);
	XAUDIO2_EFFECT_DESCRIPTOR echoDescriptor = {echoFx, true, 1};
	XAUDIO2_EFFECT_CHAIN echoChain = {1, &echoDescriptor};
	FXECHO_PARAMETERS echoParameters;
	echoParameters.WetDryMix = FXECHO_DEFAULT_WETDRYMIX * 2;
	echoParameters.Feedback = FXECHO_DEFAULT_FEEDBACK * 2;
	echoParameters.Delay = FXECHO_DEFAULT_DELAY * 2;

	IUnknown *EQFx;
	CreateFX(CLSID_FXEQ, &EQFx);
	XAUDIO2_EFFECT_DESCRIPTOR EQDescriptor = {EQFx, true, 1};
	XAUDIO2_EFFECT_CHAIN EQChain = {1, &EQDescriptor};
	FXEQ_PARAMETERS EQParameters;
	EQParameters.Gain0 = FXEQ_MIN_GAIN;
	EQParameters.FrequencyCenter0 = FXEQ_MIN_FREQUENCY_CENTER;

	IXAudio2SourceVoice *pSourceVoice;
	hr = xAudio2->CreateSourceVoice(&pSourceVoice,
	                                reinterpret_cast<WAVEFORMATEX*>(&wave),
	                                0,
	                                XAUDIO2_DEFAULT_FREQ_RATIO,
	                                nullptr,
	                                &sendList,
	                                nullptr
	);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 src voice initialization failed");
		return -1;
	}

	// Reverb
	pSourceVoice->SetEffectChain(&effectChain);
	pSourceVoice->SetEffectParameters(0, &reverbParameters, sizeof(reverbParameters));
	pSourceVoice->EnableEffect(0);

	// Echo
	pSourceVoice->SetEffectChain(&echoChain);
	pSourceVoice->SetEffectParameters(1, &echoParameters, sizeof(echoParameters));
	pSourceVoice->EnableEffect(1);

	// EQ
	pSourceVoice->SetEffectChain(&EQChain);
	pSourceVoice->SetEffectParameters(2, &EQParameters, sizeof(EQParameters));
	pSourceVoice->EnableEffect(2);

	hr = pSourceVoice->SubmitSourceBuffer(&buffer);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 src buffer initialization failed");
		return -1;
	}

	/*hr = pSourceVoice->Start(0);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 src voice start failed");
		return -1;
	}
	pSourceVoice->SetVolume(1);*/

	//ShowWindow(hwnd, nShowCmd);
	bool isRunning = true;
	while (isRunning)
	{
		MSG msg = {};
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
				break;
			}
			else if (msg.message == WM_DESTROY)
			{
				// TODO: Window closing must release its swap chain first
				SSSRenderer::Unload();
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Gamepad
		GamepadInput();

		// Render
		{
			//			using namespace SSSEngineRenderer::Directx;
			//			CommandQueue commandQueue = renderer.GetCommandQueue();
			//			auto commandList = commandQueue.GetCommandList();
			//			renderer.SetClearColor(1.0f, 0.2f, 0.5f, 0.5f, commandList.Get());
			//			renderer.ExecuteCommandList(commandList);

			SSSRenderer::Render();
		}
	}

	/*{
		using namespace SSSEngineRenderer::DirectX;
		Flush(commandQueue, fence, fenceValue, fenceEvent);
		CloseHandle(fenceEvent);
	}*/

	//SSSEngineRenderer::Unload();

	UnregisterClass(WindowClassName, hInstance);
	CloseConsole();
	return 0;
}

void GamepadInput()
{
	for (int i = 0; i < XUSER_MAX_COUNT; ++i)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		if (const DWORD dwResult = XInputGetState(i, &state); dwResult == ERROR_SUCCESS)
		{
			// Controller is connected
			const XINPUT_GAMEPAD gamepad = state.Gamepad;

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

			const float leftThumbX = gamepad.sThumbLX;
			const float leftThumbY = gamepad.sThumbLY;

			float magnitude = sqrtf(leftThumbX * leftThumbX + leftThumbY * leftThumbY);

			// Direction
			float normalizedLx = leftThumbX / magnitude;
			float normalizedLy = leftThumbY / magnitude;

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
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	freopen_s(&stream, "CONOUT$", "w", stderr);
	freopen_s(&stream, "CONIN$", "r", stdin);
}

void CloseConsole()
{
	FreeConsole();
}
