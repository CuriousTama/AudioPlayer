#pragma once

#include <array>
#include <mutex>
#include <string>
#include <xaudio2.h>
#include <xaudio2fx.h>

class Audio_System;
class Sound;

class Channel
{
private:
    class VoiceCallback : public IXAudio2VoiceCallback
    {
    protected:
        __declspec(nothrow) void __stdcall OnVoiceProcessingPassStart(UINT32 BytesRequired) override {}
        __declspec(nothrow) void __stdcall OnVoiceProcessingPassEnd() override {}
        __declspec(nothrow) void __stdcall OnStreamEnd() override {}
        __declspec(nothrow) void __stdcall OnBufferStart(void* pBufferContext) override {}
        __declspec(nothrow) void __stdcall OnBufferEnd(void* pBufferContext) override;
        __declspec(nothrow) void __stdcall OnLoopEnd(void* pBufferContext) override {}
        __declspec(nothrow) void __stdcall OnVoiceError(void* pBufferContext, HRESULT Error) override {}
    };

public:
    Channel(Audio_System& _audioSystem, const std::string& Mixer, Sound& sound);
    ~Channel();

    void play();
    void resume();
    void pause();
    void stop();

    void enable3D();
    void calculate3D(bool force = false);

    const std::string& getMixerName() const;
    const float getSpeed();
    const Sound& getSound() const;
    IXAudio2SourceVoice* getSourceVoice() { return m_pSource; }
    std::mutex& getSourceVoiceMutex() { return m_pSource_mutex; }
    XAUDIO2_BUFFER& getAudioBuffer() { return m_audio_Buffer; }
    bool isForcedStop() const { return m_forceStop; }

    void setVolume(float volume);
    void setSpeed(float speed);
    void setReverb(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& rev);
    void setReverb(const XAUDIO2FX_REVERB_PARAMETERS& rev);

private:
    Audio_System& m_audioSystem;

    inline static VoiceCallback vcb;
    XAUDIO2_BUFFER m_audio_Buffer;
    IXAudio2SourceVoice* m_pSource = nullptr;
    std::string m_mixer;
    bool m_forceStop{ false };
    Sound& m_rSound;
    std::mutex m_pSource_mutex;

    std::array<XAUDIO2_SEND_DESCRIPTOR, 2> m_sendDescriptors;
    XAUDIO2_VOICE_SENDS m_sendList;

    IUnknown* m_pReverbEffect = nullptr;
    XAUDIO2_EFFECT_DESCRIPTOR m_effects;
    XAUDIO2_EFFECT_CHAIN m_effectChain;
    IXAudio2SubmixVoice* m_reverb = nullptr;
    XAUDIO2FX_REVERB_PARAMETERS m_native{};
};
