//
// Created by Francisco Santos on 22/04/2023.
//

#include <windows.h>
#include <cstdio>
#include <iostream>
#include <xinput.h>
#include <cmath>
#include <wrl\client.h>
#include "XAudioRedist/xaudio2.h"
#include "XAudioRedist/xaudio2fx.h"
#include "XAudioRedist/xapofx.h"
#include "XAudioRedist/xapo.h"

#include "d3d12.h"
#include "d3dx12.h"
#include "dxgi1_6.h"
#include "d3dcompiler.h"
#include "DirectXMath.h"

LRESULT CALLBACK WindowMessageCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void InitConsole();
void CloseConsole();
void GamepadInput();
void InitializeDirectx12();

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

	InitializeDirectx12();

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
	buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

	// Populate the audio buffer with some simple sine wave data at 440 Hz.
	for (DWORD i = 0; i < wave.Format.nSamplesPerSec; ++i)
	{
		float t = i / (float)wave.Format.nSamplesPerSec;
		constexpr float TwoPi = 2.0f * 3.14159265358979323846264338327950288419716939937510f;

		// Write the sample to the buffer
		((short*)buffer.pAudioData)[i * 2 + 0] = (short)(sinf(t*TwoPi*440.0f) * 30000.0f); // Left channel
		((short*)buffer.pAudioData)[i * 2 + 1] = (short)(sinf(t*TwoPi*440.0f) * 30000.0f); // Right channel
	}

	IXAudio2SubmixVoice* pSubMixVoice;
	hr = xAudio2->CreateSubmixVoice(&pSubMixVoice, 2, wave.Format.nSamplesPerSec, 0, 0, nullptr, nullptr);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 submix voice initialization failed");
		return -1;
	}

	XAUDIO2_SEND_DESCRIPTOR sendDescriptors = { 0, pSubMixVoice };
	XAUDIO2_VOICE_SENDS sendList = { 1, &sendDescriptors };

	IUnknown *reverbFx;
	if (FAILED(hr = XAudio2CreateReverb(&reverbFx)))
	{
		OutputDebugStringW(L"XAudio2 reverb initialization failed");
		return -1;
	}

	XAUDIO2_EFFECT_DESCRIPTOR descriptor = {reverbFx, true, 1 };
	XAUDIO2_EFFECT_CHAIN effectChain = { 1, &descriptor };
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
	XAUDIO2_EFFECT_DESCRIPTOR echoDescriptor = { echoFx, true, 1 };
	XAUDIO2_EFFECT_CHAIN echoChain = { 1, &echoDescriptor };
	FXECHO_PARAMETERS echoParameters;
	echoParameters.WetDryMix = FXECHO_DEFAULT_WETDRYMIX * 2;
	echoParameters.Feedback = FXECHO_DEFAULT_FEEDBACK * 2;
	echoParameters.Delay = FXECHO_DEFAULT_DELAY * 2;

	IUnknown *EQFx;
	CreateFX(CLSID_FXEQ, &EQFx);
	XAUDIO2_EFFECT_DESCRIPTOR EQDescriptor = { EQFx, true, 1 };
	XAUDIO2_EFFECT_CHAIN EQChain = { 1, &EQDescriptor };
	FXEQ_PARAMETERS EQParameters;
	EQParameters.Gain0 = FXEQ_MIN_GAIN;
	EQParameters.FrequencyCenter0 = FXEQ_MIN_FREQUENCY_CENTER;

	IXAudio2SourceVoice* pSourceVoice;
	hr = xAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wave, 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &sendList, nullptr);
	if (FAILED(hr))
	{
		OutputDebugStringW(L"XAudio2 source voice initialization failed");
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

void InitializeDirectx12()
{
#if DEBUG
	// Enable the D3D12 debug layer.
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
	}
#endif

	// Query adapter
	Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
	UINT factoryFlags = 0;

#if DEBUG
	factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	if (!SUCCEEDED(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory))))
	{
		// Throw error
		throw std::runtime_error("Failed to create DXGI factory");
	}

	Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter1;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> hardwareAdapter4;

	if (bool useWarp = false)
	{
		if (!SUCCEEDED(factory->EnumWarpAdapter(IID_PPV_ARGS(&hardwareAdapter1))))
		{
			// Throw error
			throw std::runtime_error("Failed to create warp adapter");
		}
		if (!SUCCEEDED(hardwareAdapter1.As(&hardwareAdapter4)))
		{
			// Throw error
			throw std::runtime_error("Failed to create warp adapter");
		}
	}
	else
	{
		SIZE_T maxDedicatedVideoMemory = 0;
		for (UINT i = 0; factory->EnumAdapters1(i, &hardwareAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			DXGI_ADAPTER_DESC1 desc;
			hardwareAdapter1->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				continue;
			}

			if (desc.DedicatedVideoMemory > maxDedicatedVideoMemory)
			{
				maxDedicatedVideoMemory = desc.DedicatedVideoMemory;
				if (!SUCCEEDED(hardwareAdapter1.As(&hardwareAdapter4)))
				{
					// Throw error
					throw std::runtime_error("Failed to create hardware adapter");
				}
			}
		}
	}

	// Create device
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	if (!SUCCEEDED(D3D12CreateDevice(hardwareAdapter4.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&device))))
	{
		// Throw error
		throw std::runtime_error("Failed to create device");
	}

	// Enable debug messages
#if DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;

	if (SUCCEEDED(device.As(&infoQueue)))
	{
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		D3D12_MESSAGE_SEVERITY severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO
		};

		D3D12_MESSAGE_ID denyIds[] =
		{
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE
		};

		D3D12_INFO_QUEUE_FILTER newFilter = {};
		newFilter.DenyList.NumSeverities = _countof(severities);
		newFilter.DenyList.pSeverityList = severities;
		newFilter.DenyList.NumIDs = _countof(denyIds);
		newFilter.DenyList.pIDList = denyIds;

		infoQueue->PushStorageFilter(&newFilter);
	}
#endif

	// Create command queue
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.NodeMask = 0;

		if (!SUCCEEDED(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&commandQueue))))
		{
			// Throw error
			throw std::runtime_error("Failed to create command queue");
		}
	}

	// Create swap chain
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain4;
	Microsoft::WRL::ComPtr<IDXGIFactory7> factory7;

	UINT swapChainFlags = 0;
#if DEBUG
	swapChainFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	if (SUCCEEDED(CreateDXGIFactory2(swapChainFlags, IID_PPV_ARGS(&factory7))))
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = 1920;
		desc.Height = 1080;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = false;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 2;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = 0; // TODO: Check appropriate flags like allow tearing etc.

		// TODO: Should pass the handle instead
		HWND hwnd = GetActiveWindow();

		Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain1;
		if (!SUCCEEDED(factory7->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &desc, nullptr, nullptr, &swapChain1)))
		{
			// Throw error
			throw std::runtime_error("Failed to create swap chain");
		}

		if (!SUCCEEDED(swapChain1.As(&swapChain4)))
		{
			// Throw error
			throw std::runtime_error("Failed to create swap chain");
		}
	}

	// Create descriptor heap
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NumDescriptors = 2;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		desc.NodeMask = 0;

		if (!SUCCEEDED(device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&descriptorHeap))))
		{
			// Throw error
			throw std::runtime_error("Failed to create descriptor heap");
		}
	}

	// Create the render target view
	Microsoft::WRL::ComPtr<ID3D12Resource> buffers[2]; // TODO: This should be outside the scope of the function. Also the capacity should be determined somewhere else
	UINT rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
		descriptorHeap->GetCPUDescriptorHandleForHeapStart(&rtvHandle);

		for (UINT i = 0; i < 2; ++i)
		{
			Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
			if (!SUCCEEDED(swapChain4->GetBuffer(i, IID_PPV_ARGS(&backBuffer))))
			{
				// Throw error
				throw std::runtime_error("Failed to get swap chain buffer");
			}

			device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
			buffers[i] = backBuffer;
			rtvHandle.ptr += rtvDescriptorSize;
		}
	}

	// Create command allocator
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	if (!SUCCEEDED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator))))
	{
		// Throw error
		throw std::runtime_error("Failed to create command allocator");
	}

	// Create command list
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	if (!SUCCEEDED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList))))
	{
		// Throw error
		throw std::runtime_error("Failed to create command list");
	}

	// Create fence
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	UINT64 fenceValue = 0;
	{
		if (!SUCCEEDED(device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
		{
			// Throw error
			throw std::runtime_error("Failed to create fence");
		}
	}
}