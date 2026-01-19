#pragma once
#include "Channel.h"
#include "FileProcessing.h"
#include "ReverbParameters.h"
#include <array>
#include <variant>

class Channel;

using Reverb_Parameters = std::variant<XAUDIO2FX_REVERB_PARAMETERS, XAUDIO2FX_REVERB_I3DL2_PARAMETERS>;

/*
	Audio Class :
	Supported file format :
	- .WAV
	- .OGG
	- .MP3
 */
class Sound
{
private:
    friend class Channel;

public:
    enum class Simultaneous_Tracks
    {
        One,
        System_SoundLimit
    };
    using ST = Simultaneous_Tracks;
    enum class Statut
    {
        not_ready,
        stopped,
        paused,
        partially_paused,
        playing
    };

    Sound();
    Sound(const Sound&) = delete;
    Sound& operator=(const Sound&) = delete;

    // Constructor that load a file if the path is found.
    Sound(const std::string path);
    // Constructor that set if the sound can be played multiple times together load a file if the path is found.
    Sound(ST simultaneous_Tracks, const std::string path);
    ~Sound();

    // Load a sound file.
    bool loadFromFile(const std::filesystem::path);
    template<AudioHelper::DerivedOrSameType<FileProcessing> T>
    bool loadFromFileProcessing(const std::filesystem::path path);

    // Play the loaded file.
    void play();

    // Resume paused instances of this sound.
    void resume();

    // Pause all instances of this sound.
    void pause();

    //	Stop all instances of this sound.
    void stop();

    // Change Mixer output.
    void setMixer(std::string _mixerName);

    // Set sound volume.
    // volume is in percentage (min : 0.f, Max : 100.f).
    void setVolume(float volume);

    // Set if the sound replay a the end of it.
    void setLooping(bool loop) { m_loop = true; }

    // Set the sound speed.
    // speed to set in percentage (default = 100.f, min : 0.1f, Max : 200.f).
    void setSpeed(float speed);

    // Set the sound time position to given time.
    // Time as chrono duration.
    template<class _Rep, class _Period>
    void setAudioTime(const std::chrono::duration<_Rep, _Period>& _time);

    // Set 3D position.
    void setPosition(X3DAUDIO_VECTOR pos);
    void setPosition(float x, float y, float z);

    // Move 3D position.
    void movePosition(X3DAUDIO_VECTOR pos);
    void movePosition(float x, float y, float z);

    // Enable 3D position.
    void enable3D(bool b);

    // Set the max hearing distance.
    void setMaxDistance(float distance);

    // Set new reverb parameters.
    void setReverb(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& rev);
    void setReverb(const XAUDIO2FX_REVERB_PARAMETERS& rev);

    // Getters

    // Get the Mixer name.
    const std::string& getMixer() const { return m_mixer; }

    // Give the sound volume.
    float getVolume() const { return m_volume; };

    // Get if the sound loop.
    bool isLooping() const { return m_loop; }

    // Give the sound speed.
    float getSpeed() const { return m_speed; }

    // Give the sound duration in seconds.
    double getAudioDuration() const;

    // Give the sound current time.
    // current time (seconds), if none launched (return 0), if multiple launched return value of the first instance.
    double getAudioTime() const;

    // Get if a file is linked.
    bool isLoaded() const { return m_loaded; }

    // Get the sound status.
    Sound::Statut getStatus() const;

    // Get the emitter position.
    // @param T : template with constructor (float, float, float).
    template<AudioHelper::VectorConstructible T = X3DAUDIO_VECTOR>
    T getPosition() const;

    // Get if the 3D calculation is activated.
    bool is3D() const;

    // Get the max hearing distance.
    float getMaxDistance() const;

    // Get dsp information.
    const X3DAUDIO_DSP_SETTINGS& getDSPInfos() const;

private:
    // Add a channel to the active channel List.
    void addChannel(Channel& channel);

    // Remove a channel from the active channel List.
    void removeChannel(Channel& channel);

    // Get reverb parameters.
    Reverb_Parameters& getReverb() { return m_reverb; }

    std::shared_ptr<FileProcessing> getFileProcessing() { return m_file; }
    X3DAUDIO_EMITTER& getEmitter() { return m_X3DEmitter; }
    std::vector<std::reference_wrapper<Channel>> getActiveChannelPtrs() { return m_channelHandles; }
    std::mutex& channelList_mutex() { return m_channelHandles_mutex; }

    X3DAUDIO_DSP_SETTINGS& getDSPInfos() { return m_DSPSettings; }

private:
    mutable std::mutex m_channelHandles_mutex;

    std::string m_mixer{ "master" };
    std::vector<std::reference_wrapper<Channel>> m_channelHandles;
    bool m_loaded{ false };
    float m_volume{ 100.f };
    bool m_loop{ false };
    float m_speed{ 100.f };
    bool m_is3D{ false };
    Simultaneous_Tracks m_st{ ST::System_SoundLimit };
    std::shared_ptr<FileProcessing> m_file;

    // 3D
    Reverb_Parameters m_reverb{ ReverbParameters::Default };
    X3DAUDIO_EMITTER m_X3DEmitter{ 0 };
    X3DAUDIO_CONE m_emitterCone;
    std::array<X3DAUDIO_DISTANCE_CURVE_POINT, 10> m_volumePoints{};
    std::array<X3DAUDIO_DISTANCE_CURVE_POINT, 10> m_reverbPoints{};
    X3DAUDIO_DISTANCE_CURVE m_volumeCurve;
    X3DAUDIO_DISTANCE_CURVE m_reverbCurve;

    std::unique_ptr<float[]> m_matrix;
    X3DAUDIO_DSP_SETTINGS m_DSPSettings = { 0 };
};

// ----------------------------------------------
template<AudioHelper::DerivedOrSameType<FileProcessing> T>
inline bool Sound::loadFromFileProcessing(const std::filesystem::path path)
{
    m_loaded = false;

    if (std::filesystem::exists(path))
    {
        stop();
        m_file.reset();

        Audio_System* audioSystem = Audio_System::getExistingInstance();
        bool alreadyExist = audioSystem->containFile(path);
        m_file = audioSystem->getOrCreateFile<T>(path);

        if (alreadyExist)
        {
            m_loaded = true;
        }
        else
        {
            m_loaded = m_file->load(path.string());
        }

        audioSystem->updateBuffersLife();
    }

    if (!m_loaded)
    {
        std::string error("[error] : Failed to open : " + path.string() + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
        throw std::exception(error.c_str());
    }

    return m_loaded;
}

// ----------------------------------------------
template<class _Rep, class _Period>
inline void Sound::setAudioTime(const std::chrono::duration<_Rep, _Period>& _time)
{
    if (!m_file || m_channelHandles.empty())
        return;

    std::chrono::milliseconds time = std::chrono::duration_cast<std::chrono::milliseconds>(_time);
    if ((time.count() / 1000.0) > this->getAudioDuration())
    {
        m_file->setPlayTime(m_channelHandles.front(), this->getAudioDuration());
    }
    else if (time.count() <= 0)
    {
        m_file->setPlayTime(m_channelHandles.front(), 0.0);
    }
    else
    {
        m_file->setPlayTime(m_channelHandles.front(), time.count() / 1000.0);
    }
}

// ----------------------------------------------
template<AudioHelper::VectorConstructible T>
inline T Sound::getPosition() const
{
    if constexpr (std::is_same_v<T, decltype(m_X3DEmitter.Position)>)
    {
        return m_X3DEmitter.Position;
    }

    return T(m_X3DEmitter.Position.x, m_X3DEmitter.Position.y, m_X3DEmitter.Position.z);
}