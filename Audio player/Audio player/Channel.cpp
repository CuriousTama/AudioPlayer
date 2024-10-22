#include "Channel.h"
#include "Audio_System.h"
#include "ReverbParameters.h"
#include "Sound.h"

void __stdcall Channel::VoiceCallback::OnBufferEnd(void* pBufferContext)
{
    if (pBufferContext)
    {
        Channel& chan = *static_cast<Channel*>(pBufferContext);

        if (chan.m_rSound.getFileProcessing()->onBufferEnd(chan))
        {
            chan.stop();
            chan.m_rSound.removeChannel(chan);

            Audio_System* audioSystem = Audio_System::getExistingInstance();
            assert(audioSystem != nullptr && std::format("Audio_System should exist to execute this. [{}]", __FUNCTION__).c_str());
            audioSystem->removeChannel(chan);
        }
    }
}

Channel::Channel(Audio_System& _audioSystem, const std::string& Mixer, Sound& sound)
    : m_audioSystem(_audioSystem)
    , m_rSound(sound)
{
    ZeroMemory(&m_audio_Buffer, sizeof(m_audio_Buffer));
    m_audio_Buffer.pContext = this;
    m_mixer = Mixer;

    m_sendDescriptors[0].Flags = XAUDIO2_SEND_USEFILTER; // LPF direct-path
    m_sendDescriptors[1].Flags = XAUDIO2_SEND_USEFILTER; // LPF reverb-path -- omit for better performance at the cost of less realistic occlusion

    if (m_mixer != "master" && m_audioSystem.getSubVoices().contains(m_mixer))
    {
        m_sendDescriptors[0].pOutputVoice = m_audioSystem.getSubVoices()[m_mixer];
    }
    else
    {
        m_mixer = "master";
        m_sendDescriptors[0].pOutputVoice = m_audioSystem.getMasterVoice();
    }

    m_sendList.SendCount = 1;
    m_sendList.pSends = m_sendDescriptors.data();

    m_audioSystem.getEngine()->CreateSourceVoice(&m_pSource, &m_rSound.getFileProcessing()->getFormat(), XAUDIO2_VOICE_USEFILTER, 2.0f, &vcb, &m_sendList);
    XAudio2CreateReverb(&m_pReverbEffect);

    m_effects.pEffect = m_pReverbEffect;
    m_effects.InitialState = true;
    m_effects.OutputChannels = 1;

    m_effectChain.EffectCount = 1;
    m_effectChain.pEffectDescriptors = &m_effects;

    const Reverb_Parameters& soundReverb = m_rSound.getReverb();
    if (std::holds_alternative<XAUDIO2FX_REVERB_PARAMETERS>(soundReverb))
    {
        setReverb(std::get<XAUDIO2FX_REVERB_PARAMETERS>(soundReverb));
    }
    else if (std::holds_alternative<XAUDIO2FX_REVERB_I3DL2_PARAMETERS>(soundReverb))
    {
        setReverb(std::get<XAUDIO2FX_REVERB_I3DL2_PARAMETERS>(soundReverb));
    }
}

Channel::~Channel()
{
    if (m_pSource)
    {
        if (m_audio_Buffer.AudioBytes >= 4)
        {
            this->stop();
        }
        else
        {
            m_rSound.removeChannel(*this);
        }

        bool DeactivatedEveryWhere = false;
        do
        {
            std::lock_guard<std::mutex> lock(m_rSound.channelList_mutex());
            if (m_rSound.getActiveChannelPtrs().empty())
            {
                DeactivatedEveryWhere = true;
            }
            else
            {
                DeactivatedEveryWhere = true;
                for (auto& _channel : m_rSound.getActiveChannelPtrs())
                {
                    if (this == &_channel.get())
                    {
                        DeactivatedEveryWhere = false;
                    }
                }
            }

        } while (!DeactivatedEveryWhere);

        m_pSource->Stop();
        m_pSource->DestroyVoice();
        m_pSource = nullptr;
    }

    std::lock_guard<std::mutex> lock(m_pSource_mutex);
}

void Channel::play()
{
    m_audio_Buffer.PlayBegin = 0;
    m_pSource->Discontinuity();

    m_rSound.addChannel(*this);

    m_rSound.getFileProcessing()->play(*this);

    m_audio_Buffer.pAudioData = m_rSound.getFileProcessing()->get_pData(*this);
    m_audio_Buffer.AudioBytes = m_rSound.getFileProcessing()->get_nBytes(*this);

    std::lock_guard<std::mutex> lock(m_pSource_mutex);
    m_pSource->SubmitSourceBuffer(&m_audio_Buffer, nullptr);
    m_pSource->SetVolume(m_rSound.getVolume() / 100.f);
    m_pSource->SetFrequencyRatio(m_rSound.getSpeed() / 100.f);
    this->enable3D();
    m_pSource->Start();
}

void Channel::resume()
{
    std::lock_guard<std::mutex> lock(m_pSource_mutex);
    m_pSource->SetFrequencyRatio(m_rSound.getSpeed() / 100.f);
}

void Channel::pause()
{
    std::lock_guard<std::mutex> lock(m_pSource_mutex);
    m_pSource->SetFrequencyRatio(0.f); // speed
}

void Channel::stop()
{
    std::lock_guard<std::mutex> lock(m_pSource_mutex);

    m_forceStop = true;
    m_pSource->Stop();
    m_pSource->FlushSourceBuffers(); // will call VoiceCallback::onBufferEnd()
}

const std::string& Channel::getMixerName() const
{
    return m_rSound.getMixer();
}

void Channel::setVolume(float volume)
{
    std::lock_guard<std::mutex> lock(m_pSource_mutex);
    m_pSource->SetVolume(volume / 100.f);
}

void Channel::setSpeed(float speed)
{
    std::lock_guard<std::mutex> lock(m_pSource_mutex);
    m_pSource->SetFrequencyRatio(speed / 100.f);
}

const float Channel::getSpeed()
{
    std::lock_guard<std::mutex> lock(m_pSource_mutex);

    float freq = 1.f;
    m_pSource->GetFrequencyRatio(&freq);
    return freq;
}

const Sound& Channel::getSound() const
{
    return m_rSound;
}

void Channel::enable3D()
{
    if (m_rSound.is3D())
    {
        this->calculate3D();
    }
    else
    {
        X3DAUDIO_VECTOR LastPos = m_rSound.getPosition();
        m_rSound.getEmitter().Position = m_audioSystem.getListener().Position;

        this->calculate3D(true);
        m_pSource->SetFrequencyRatio((m_rSound.getSpeed() / 100.f));

        m_rSound.getEmitter().Position = LastPos;
    }
}

void Channel::calculate3D(bool force)
{
    if (m_rSound.is3D() || force)
    {
        static DWORD dwCalcFlags = X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_REVERB | X3DAUDIO_CALCULATE_EMITTER_ANGLE;

        X3DAudioCalculate(m_audioSystem.getX3DAUDIO_Handle(), &m_audioSystem.getListener(), &m_rSound.getEmitter(), dwCalcFlags, &m_rSound.getDSPInfos());

        m_pSource->SetOutputMatrix(m_sendDescriptors[0].pOutputVoice, m_rSound.getFileProcessing()->getFormat().nChannels, m_audioSystem.getVoiceDetails().InputChannels, m_rSound.getDSPInfos().pMatrixCoefficients);
        //m_pSource->SetFrequencyRatio((m_rSound.m_speed / 100.f) * m_rSound.m_DSPSettings.DopplerFactor);

        if (m_reverb)
        {
            m_pSource->SetOutputMatrix(m_reverb, m_rSound.getFileProcessing()->getFormat().nChannels, 1, &m_rSound.getDSPInfos().ReverbLevel);
        }
    }
}

void Channel::setReverb(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& rev)
{
    if (rev != ReverbParameters::Default)
    {
        if (!m_reverb)
        {
            m_audioSystem.getEngine()->CreateSubmixVoice(&m_reverb, 1, 44100, XAUDIO2_VOICE_USEFILTER, 0, nullptr, &m_effectChain);
        }

        if (!m_reverb)
        {
            std::string error("[error] : fail to create reverb. \n[File] " __FILE__ "\n[Line] " + std::to_string(__LINE__));
            throw std::exception(error.c_str());
        }

        ReverbConvertI3DL2ToNative(&rev, &m_native);

        //Override rear for mono
        m_native.RearDelay = 5;
        m_reverb->SetEffectParameters(0, &m_native, sizeof(m_native));

        m_sendDescriptors[1].pOutputVoice = m_reverb;
        m_sendList.SendCount = 2;
    }
    else
    {
        m_sendList.SendCount = 1;
    }

    m_pSource->SetOutputVoices(&m_sendList);
    this->calculate3D();
}

void Channel::setReverb(const XAUDIO2FX_REVERB_PARAMETERS& rev)
{
    if (!m_reverb)
    {
        m_audioSystem.getEngine()->CreateSubmixVoice(&m_reverb, 1, 44100, XAUDIO2_VOICE_USEFILTER, 0, nullptr, &m_effectChain);
    }

    if (!m_reverb)
    {
        std::string error("[error] : fail to create reverb. \n[File] " __FILE__ "\n[Line] " + std::to_string(__LINE__));
        throw std::exception(error.c_str());
    }

    m_native = rev;
    m_reverb->SetEffectParameters(0, &m_native, sizeof(m_native));

    m_sendDescriptors[1].pOutputVoice = m_reverb;
    m_sendList.SendCount = 2;

    m_pSource->SetOutputVoices(&m_sendList);
    this->calculate3D();
}
