#include "Audio_System.h"
#include "Channel.h"
#include "Sound.h"
#include <iostream>

#include "FileProcessing_MP3.h"
#include "FileProcessing_OGG.h"
#include "FileProcessing_WAV.h"

Audio_System::Audio_System()
{
    assert(m_instance == nullptr && "There should be only one instance of Audio_System alive at the same time");

    // constructor
    std::ignore = CoInitializeEx(NULL, COINIT_MULTITHREADED);

    XAudio2Create(&m_pEngine);
    m_pEngine->CreateMasteringVoice(&m_pMaster, 2, 44100);

    DWORD dwChannelMask;
    m_pMaster->GetChannelMask(&dwChannelMask);
    X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, m_X3DInstance);

    m_X3DListener.OrientFront.z = 1.0f;
    m_X3DListener.OrientTop.y = 1.0f;
    m_X3DListener.pCone = nullptr;
    // //

    registerDefaultProcessors();

    m_instance = this;
}

Audio_System::~Audio_System()
{
    m_pEngine->StopEngine();
    m_pMaster->DestroyVoice();
    
    for (auto [name, voice] : m_subVoices)
    {
        voice->DestroyVoice();
    }

    m_activeChannelPtrs.clear();
    m_buffers.clear();
    m_pEngine->Release();

    CoUninitialize();

    std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
    m_instance = nullptr;
}

bool Audio_System::addSubMixer(const std::string& _subMixerName)
{
    if (!m_subVoices.contains(_subMixerName) && _subMixerName != "master")
    {
        m_pEngine->CreateSubmixVoice(&m_subVoices[_subMixerName], 2, 44100);
        return true;
    }

    return false;
}

bool Audio_System::removeSubMixer(const std::string& _subMixerName)
{
    if (m_subVoices.contains(_subMixerName) && _subMixerName != "master")
    {
        m_subVoices[_subMixerName]->DestroyVoice();
        m_subVoices.erase(_subMixerName);

        while (!m_activeChannelPtrs[_subMixerName].empty())
        {
            for (auto& c : m_activeChannelPtrs[_subMixerName])
            {
                removeChannel(*c);
            }
        }

        m_activeChannelPtrs.erase(_subMixerName);
        return true;
    }

    return false;
}

bool Audio_System::doesSubMixerExist(const std::string& _subMixerName) const
{
    if (_subMixerName == "master")
    {
        return true;
    }

    return m_subVoices.contains(_subMixerName);
}

void Audio_System::setMasterVolume(float volume)
{
    m_pMaster->SetVolume(std::clamp(volume / 100.f, 0.f, 1.f));
}

float Audio_System::getMasterVolume() const
{
    float volume = 0.f;
    m_pMaster->GetVolume(&volume);

    return volume * 100.f;
}

void Audio_System::setSubMixerVolume(const std::string& _subMixerName, float _volume)
{
    _volume = std::clamp(_volume, 0.f, 100.f);

    if (m_subVoices.contains(_subMixerName))
    {
        m_subVoices[_subMixerName]->SetVolume(_volume / 100.f);
    }

    if (_subMixerName == "master")
    {
        setMasterVolume(_volume);
    }
}

float Audio_System::getSubMixerVolume(const std::string& _subMixerName) const
{
    float volume = 0.f;

    if (m_subVoices.contains(_subMixerName))
    {
        m_subVoices.at(_subMixerName)->GetVolume(&volume);
    }
    if (_subMixerName == "master")
    {
        return getMasterVolume();
    }

    return volume * 100.f;
}

void Audio_System::setListenerPosition(float _x, float _y, float _z)
{
    m_X3DListener.Position = X3DAUDIO_VECTOR{ _x, _y, _z };
    recalculate3D();
}

void Audio_System::setListenerPosition(X3DAUDIO_VECTOR _pos)
{
    m_X3DListener.Position = _pos;
    recalculate3D();
}

void Audio_System::moveListenerPosition(float _x, float _y, float _z)
{
    m_X3DListener.Position.x += _x;
    m_X3DListener.Position.y += _y;
    m_X3DListener.Position.z += _z;
    recalculate3D();
}

void Audio_System::moveListenerPosition(X3DAUDIO_VECTOR _pos)
{
    m_X3DListener.Position.x += _pos.x;
    m_X3DListener.Position.y += _pos.y;
    m_X3DListener.Position.z += _pos.z;
    recalculate3D();
}

void Audio_System::setListenerOrientation(X3DAUDIO_VECTOR _forward, X3DAUDIO_VECTOR _topDirection)
{
    m_X3DListener.OrientFront = _forward;
    m_X3DListener.OrientTop = _topDirection;
    recalculate3D();
}

void Audio_System::setListenerOrientation(X3DAUDIO_VECTOR _forward, float _topDir_x, float _topDir_y, float _topDir_z)
{
    setListenerOrientation(_forward, { _topDir_x, _topDir_y, _topDir_z });
}

void Audio_System::setListenerOrientation(float _fwd_x, float _fwd_y, float _fwd_z, X3DAUDIO_VECTOR _topDirection)
{
    setListenerOrientation({ _fwd_x, _fwd_y, _fwd_z }, _topDirection);
}

void Audio_System::setListenerOrientation(float _fwd_x, float _fwd_y, float _fwd_z, float _topDir_x, float _topDir_y, float _topDir_z)
{
    setListenerOrientation({ _fwd_x, _fwd_y, _fwd_z }, { _topDir_x, _topDir_y, _topDir_z });
}

void Audio_System::setListenerForward(X3DAUDIO_VECTOR forward)
{
    m_X3DListener.OrientFront = forward;
    recalculate3D();
}

void Audio_System::setListenerForward(float _x, float _y, float _z)
{
    setListenerForward({ _x, _y, _z });
}

void Audio_System::setListenerTop(X3DAUDIO_VECTOR _top)
{
    m_X3DListener.OrientTop = _top;
    recalculate3D();
}

void Audio_System::setListenerTop(float _x, float _y, float _z)
{
    setListenerTop({ _x, _y, _z });
}

void Audio_System::setDefaultMaxDistance(float maxDistance)
{
    m_DefaultMaxDistance = maxDistance;
}

void Audio_System::removeChannel(const Channel& channel)
{
    std::lock_guard<std::mutex> lock(m_ChannelList_mutex);

    const std::string Target_Mixer = channel.getMixerName();

    auto it = std::find_if(std::begin(m_activeChannelPtrs[Target_Mixer]), std::end(m_activeChannelPtrs[Target_Mixer]),
                           [&channel](const std::unique_ptr<Channel>& pChan)
                           {
                               return &channel == pChan.get();
                           });

    if (it != std::end(m_activeChannelPtrs[Target_Mixer]))
    {
        m_activeChannelPtrs[Target_Mixer].erase(it);
    }
}

bool Audio_System::containFile(const std::filesystem::path& path) const
{
    return m_buffers.find(path) != m_buffers.end();
}

void Audio_System::registerDefaultProcessors()
{
    m_factory.registerExtension<FileProcessing_WAV>(".wav");
    m_factory.registerExtension<FileProcessing_OGG>(".ogg");
    m_factory.registerExtension<FileProcessing_MP3>(".mp3");
}

void Audio_System::play(Sound& sound)
{
    std::lock_guard<std::mutex> lock(m_ChannelList_mutex);

    std::string Target_Mixer(sound.getMixer());

    if (Target_Mixer != "master" && !m_subVoices.contains(Target_Mixer))
    {
        sound.setMixer("master");
        Target_Mixer = sound.getMixer();
    }

    if (Target_Mixer == "master" || m_subVoices.contains(Target_Mixer))
    {
        if (m_activeChannelPtrs[Target_Mixer].size() < static_cast<size_t>(m_MaxChannelsPerMixer))
        {
            m_activeChannelPtrs[Target_Mixer].emplace_back(std::make_unique<Channel>(*this, Target_Mixer, sound));
            try
            {
                m_activeChannelPtrs[Target_Mixer].back()->play();
            }
            catch (...)
            {
                m_activeChannelPtrs[Target_Mixer].pop_back();
                throw;
            }
        }
    }
}

XAUDIO2_VOICE_DETAILS Audio_System::getVoiceDetails()
{
    XAUDIO2_VOICE_DETAILS deviceDetails;
    m_pMaster->GetVoiceDetails(&deviceDetails);
    return deviceDetails;
}

void Audio_System::updateBuffersLife()
{
    std::vector<std::filesystem::path> erraseList;
    for (const auto& [path, buffer] : m_buffers)
    {
        if (buffer.use_count() <= 1)
        {
            erraseList.push_back(path);
        }
    }

    for (const auto& path : erraseList)
    {
        m_buffers.erase(path);
    }
}

void Audio_System::recalculate3D()
{
    for (auto& [mixer, Vec_channel] : m_activeChannelPtrs)
    {
        for (auto& channel : Vec_channel)
        {
            channel->calculate3D();
        }
    }
}