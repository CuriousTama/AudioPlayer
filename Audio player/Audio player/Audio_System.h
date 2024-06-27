#pragma once

#pragma comment(lib, "xaudio2.lib")
#include <windows.h> // need to be included first (include order for xaudio2.h and x3daudio.h)

#include <cstddef>
#include <filesystem>
#include <map>
#include <mutex>
#include <vector>
#include <x3daudio.h>
#include <xaudio2.h>

#include "AudioHelper.h"
#include "FileProcessingFactory.h"

class Channel;
class FileProcessing;
class Sound;

class Audio_System final
{
    friend class Channel;
    friend class Sound;

public:
    Audio_System();
    Audio_System(const Audio_System&) = delete;
    Audio_System& operator=(const Audio_System&) = delete;
    Audio_System(const Audio_System&&) = delete;
    Audio_System& operator=(const Audio_System&&) = delete;
    ~Audio_System();

    // Get the existing instance of Audio_System.
    static Audio_System* getExistingInstance() { return m_instance; }

    // Add a new sub-mixer.
    bool addSubMixer(const std::string& _subMixerName);
    // Remove a sub-mixer.
    bool removeSubMixer(const std::string& _subMixerName);
    // check if a sub-mixer exist.
    bool doesSubMixerExist(const std::string& _subMixerName) const;

    // Set the volume of the master Mixer.
    void setMasterVolume(float _volume);
    // Get the volume of the master Mixer
    float getMasterVolume() const;
    // Set the volume of a sub-mixer.
    void setSubMixerVolume(const std::string& _subMixerName, float _volume);
    // Get the volume of a sub-mixer.
    float getSubMixerVolume(const std::string& _subMixerName) const;

    // Set 3D position of the listener.
    void setListenerPosition(float _x, float _y, float _z);
    void setListenerPosition(X3DAUDIO_VECTOR _pos);

    // Move 3D position of the listener.
    void moveListenerPosition(float _x, float _y, float _z);
    void moveListenerPosition(X3DAUDIO_VECTOR _pos);

    // Give the listener position, x, y, z.
    // @param T : template with constructor (float, float, float).
    template<AudioHelper::VectorConstructible T = X3DAUDIO_VECTOR>
    T getListenerPosition() const;

    // Set the listener Orientation.
    void setListenerOrientation(X3DAUDIO_VECTOR _forward, X3DAUDIO_VECTOR _topDirection);
    void setListenerOrientation(X3DAUDIO_VECTOR _forward, float _topDir_x, float _topDir_y, float _topDir_z);
    void setListenerOrientation(float _fwd_x, float _fwd_y, float _fwd_z, X3DAUDIO_VECTOR _topDirection);
    void setListenerOrientation(float _fwd_x, float _fwd_y, float _fwd_z, float _topDir_x, float _topDir_y, float _topDir_z);

    // Set the listener Forward (Orientation).
    void setListenerForward(X3DAUDIO_VECTOR _forward);
    void setListenerForward(float _x, float _y, float _z);

    // Give the listener Forward (Orientation), x, y, z.
    // @param T : template with constructor (float, float, float).
    template<AudioHelper::VectorConstructible T = X3DAUDIO_VECTOR>
    T getListenerForward() const;

    // Set the listener Top (Orientation).
    void setListenerTop(X3DAUDIO_VECTOR top);
    void setListenerTop(float x, float y, float z);

    // Give the listener Top (Orientation), x, y, z.
    // @param T : template with constructor (float, float, float).
    template<AudioHelper::VectorConstructible T = X3DAUDIO_VECTOR>
    T getListenerTop() const;

    // Set the listener Max hearing distance.
    void setDefaultMaxDistance(float maxDistance);

    // Give the listener Max hearing distance.
    float getDefaultMaxDistance() const { return m_DefaultMaxDistance; }

private:
    bool containFile(const std::filesystem::path& path) const;

    // add constraint / static assert
    template<AudioHelper::DerivedOrSameType<FileProcessing> T = FileProcessing>
    std::shared_ptr<FileProcessing> getOrCreateFile(const std::filesystem::path& path);

    std::map<std::string, IXAudio2SubmixVoice*>& getSubVoices() { return m_subVoices; }
    IXAudio2* getEngine() { return m_pEngine; }
    IXAudio2MasteringVoice* getMasterVoice() { return m_pMaster; }
    X3DAUDIO_LISTENER& getListener() { return m_X3DListener; }
    X3DAUDIO_HANDLE& getX3DAUDIO_Handle() { return m_X3DInstance; }
    XAUDIO2_VOICE_DETAILS getVoiceDetails();

    void registerDefaultProcessors();
    void recalculate3D();
    void removeChannel(const Channel& channel);
    void play(Sound& sound);
    void updateBuffersLife();

private:
    inline static Audio_System* m_instance{ nullptr };

    FileProcessingFactory m_factory;
    int m_MaxChannelsPerMixer{ 64 };
    float m_DefaultMaxDistance{ 100 };
    IXAudio2MasteringVoice* m_pMaster{ nullptr }; // need only for engine ? can't interact directly
    X3DAUDIO_LISTENER m_X3DListener;
    X3DAUDIO_HANDLE m_X3DInstance;
    IXAudio2* m_pEngine{ nullptr }; // thread etc... ?
    std::mutex m_ChannelList_mutex;
    std::map<std::string, std::vector<std::unique_ptr<Channel>>> m_activeChannelPtrs;
    std::map<std::filesystem::path, std::shared_ptr<FileProcessing>> m_buffers;
    std::map<std::string, IXAudio2SubmixVoice*> m_subVoices;
};

////////////////////////
template<AudioHelper::VectorConstructible T>
inline T Audio_System::getListenerPosition() const
{
    if constexpr (std::is_same_v<T, decltype(m_X3DListener.Position)>)
    {
        return m_X3DListener.Position;
    }

    return T(m_X3DListener.Position.x, m_X3DListener.Position.y, m_X3DListener.Position.z);
}

////////////////////////
template<AudioHelper::VectorConstructible T>
inline T Audio_System::getListenerForward() const
{
    if constexpr (std::is_same_v<T, decltype(m_X3DListener.OrientFront)>)
    {
        return m_X3DListener.OrientFront;
    }

    return T(m_X3DListener.OrientFront.x, m_X3DListener.OrientFront.y, m_X3DListener.OrientFront.z);
}

////////////////////////
template<AudioHelper::VectorConstructible T>
inline T Audio_System::getListenerTop() const
{
    if constexpr (std::is_same_v<T, decltype(m_X3DListener.OrientTop)>)
    {
        return m_X3DListener.OrientTop;
    }

    return T(m_X3DListener.OrientTop.x, m_X3DListener.OrientTop.y, m_X3DListener.OrientTop.z);
}

////////////////////////
template<AudioHelper::DerivedOrSameType<FileProcessing> T>
inline std::shared_ptr<FileProcessing> Audio_System::getOrCreateFile(const std::filesystem::path& path)
{
    auto it = m_buffers.find(path);

    if (it != m_buffers.end() && it->second)
    {
        return it->second;
    }

    if constexpr (std::is_same_v<T, FileProcessing>)
    {
        std::string extention = path.extension().string();

        if (!m_factory.contains(extention))
        {
            // error
            return nullptr;
        }

        m_buffers[path] = m_factory.create(extention);
    }
    else
    {
        m_buffers[path] = std::make_unique<T>();
    }

    return m_buffers[path];
}
