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

#include <cmath> // TODO: Sin in math library
#include <windows.h>
#include "xapofxredist.h"
#include "xaudio2fxredist.h"
#include "xaudio2redist.h"
#include <wrl/client.h>
#include "Audio.h"

namespace SSSEngine::SSSAudio
{
    // INVESTIGATE: What to do when failing?
    void Init()
    {
        HRESULT hr;
        Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
        hr = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
        if(FAILED(hr))
        {
            OutputDebugStringW(L"XAudio2 initialization failed");
            return;
        }

        IXAudio2MasteringVoice *pMasteringVoice; // NOLINT(*-init-variables)
        hr = xAudio2->CreateMasteringVoice(&pMasteringVoice);
        if(FAILED(hr))
        {
            OutputDebugStringW(L"XAudio2 mastering voice initialization failed");
            return;
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
        for(DWORD i = 0; i < wave.Format.nSamplesPerSec; ++i)
        {
            float t = static_cast<float>(i) / static_cast<float>(wave.Format.nSamplesPerSec);
            constexpr float twoPi = 2.0f * 3.14159265358979323846264338327950288419716939937510f;

            // ReSharper disable CppCStyleCast NOLINTBEGIN(*-pro-type-cstyle-cast)
            // Write the sample to the buffer
            ((BYTE *)buffer.pAudioData)[i * 2 + 0] = static_cast<BYTE>(sinf(t * twoPi * 440.0f) * 30000.0f); // Left
                                                                                                             // channel
            ((BYTE *)buffer.pAudioData)[i * 2 + 1] = static_cast<BYTE>(sinf(t * twoPi * 440.0f) * 30000.0f); // Right
                                                                                                             // channel
            // ReSharper restore CppCStyleCast NOLINTEND(*-pro-type-cstyle-cast)
        }

        IXAudio2SubmixVoice *pSubMixVoice; // NOLINT(*-init-variables)
        hr = xAudio2->CreateSubmixVoice(&pSubMixVoice, 2, wave.Format.nSamplesPerSec, 0, 0, nullptr, nullptr);
        if(FAILED(hr))
        {
            OutputDebugStringW(L"XAudio2 submix voice initialization failed");
            return;
        }

        XAUDIO2_SEND_DESCRIPTOR sendDescriptors = {0, pSubMixVoice};
        XAUDIO2_VOICE_SENDS sendList = {1, &sendDescriptors};

        IUnknown *reverbFx; // NOLINT(*-init-variables)
        hr = XAudio2CreateReverb(&reverbFx);
        if(FAILED(hr))
        {
            OutputDebugStringW(L"XAudio2 reverb initialization failed");
            return;
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

        IUnknown *echoFx; // NOLINT(*-init-variables)
        CreateFX(CLSID_FXEcho, &echoFx);
        XAUDIO2_EFFECT_DESCRIPTOR echoDescriptor = {echoFx, true, 1};
        XAUDIO2_EFFECT_CHAIN echoChain = {1, &echoDescriptor};
        FXECHO_PARAMETERS echoParameters;
        echoParameters.WetDryMix = FXECHO_DEFAULT_WETDRYMIX * 2;
        echoParameters.Feedback = FXECHO_DEFAULT_FEEDBACK * 2;
        echoParameters.Delay = FXECHO_DEFAULT_DELAY * 2;

        IUnknown *eqFx; // NOLINT(*-init-variables)
        CreateFX(CLSID_FXEQ, &eqFx);
        XAUDIO2_EFFECT_DESCRIPTOR eqDescriptor = {eqFx, true, 1};
        XAUDIO2_EFFECT_CHAIN eqChain = {1, &eqDescriptor};
        FXEQ_PARAMETERS eqParameters;
        eqParameters.Gain0 = FXEQ_MIN_GAIN;
        eqParameters.FrequencyCenter0 = FXEQ_MIN_FREQUENCY_CENTER;

        IXAudio2SourceVoice *pSourceVoice; // NOLINT(*-init-variables)
        hr = xAudio2->CreateSourceVoice(
            &pSourceVoice, reinterpret_cast<WAVEFORMATEX *>(&wave), 0, XAUDIO2_DEFAULT_FREQ_RATIO, nullptr, &sendList, nullptr);
        if(FAILED(hr))
        {
            OutputDebugStringW(L"XAudio2 src voice initialization failed");
            return;
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
        pSourceVoice->SetEffectChain(&eqChain);
        pSourceVoice->SetEffectParameters(2, &eqParameters, sizeof(eqParameters));
        pSourceVoice->EnableEffect(2);

        hr = pSourceVoice->SubmitSourceBuffer(&buffer);
        if(FAILED(hr))
        {
            OutputDebugStringW(L"XAudio2 src buffer initialization failed");
            return;
        }
        /*hr = pSourceVoice->Start(0);
        if (FAILED(hr))
        {
                OutputDebugStringW(L"XAudio2 src voice start failed");
                return -1;
        }
        pSourceVoice->SetVolume(1);*/
    }
} // namespace SSSEngine::SSSAudio
