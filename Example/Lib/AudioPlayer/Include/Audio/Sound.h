#pragma once
#include "FileProcessing.h"
#include <variant>

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

public:
	enum class Simultaneous_Tracks { One, System_SoundLimit };
	using ST = Simultaneous_Tracks;
	enum class Statut { not_ready, stopped, paused, partially_paused, playing };

	/*
		List of presets reverbs from xaudio2fx
	*/
	struct Reverb
	{
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Default{ 100,         -10000,     0,0.0f,  1.00f, 0.50f, -10000,0.020f, -10000, 0.040f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Generic{ 100,         -1000 ,  -100,0.0f,  1.49f, 0.83f,  -2602,0.007f,    200, 0.011f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Paddedcell{ 100,      -1000 , -6000,0.0f,  0.17f, 0.10f,  -1204,0.001f,    207, 0.002f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Room{ 100,            -1000 ,  -454,0.0f,  0.40f, 0.83f,  -1646,0.002f,     53, 0.003f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Bathroom{ 100,        -1000 , -1200,0.0f,  1.49f, 0.54f,   -370,0.007f,   1030, 0.011f, 100.0f,  60.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Livingroom{ 100,      -1000 , -6000,0.0f,  0.50f, 0.10f,  -1376,0.003f,  -1104, 0.004f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Stoneroom{ 100,       -1000 ,  -300,0.0f,  2.31f, 0.64f,   -711,0.012f,     83, 0.017f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Auditorium{ 100,      -1000 ,  -476,0.0f,  4.32f, 0.59f,   -789,0.020f,   -289, 0.030f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Concerthall{ 100,     -1000 ,  -500,0.0f,  3.92f, 0.70f,  -1230,0.020f,     -2, 0.029f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Cave{ 100,            -1000 ,     0,0.0f,  2.91f, 1.30f,   -602,0.015f,   -302, 0.022f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Arena{ 100,           -1000 ,  -698,0.0f,  7.24f, 0.33f,  -1166,0.020f,     16, 0.030f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Hangar{ 100,          -1000 , -1000,0.0f, 10.05f, 0.23f,   -602,0.020f,    198, 0.030f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Carpetedhallway{ 100, -1000 , -4000,0.0f,  0.30f, 0.10f,  -1831,0.002f,  -1630, 0.030f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Hallway{ 100,         -1000 ,  -300,0.0f,  1.49f, 0.59f,  -1219,0.007f,    441, 0.011f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Stonecorridor{ 100,   -1000 ,  -237,0.0f,  2.70f, 0.79f,  -1214,0.013f,    395, 0.020f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Alley{ 100,           -1000 ,  -270,0.0f,  1.49f, 0.86f,  -1204,0.007f,     -4, 0.011f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Forest{ 100,          -1000 , -3300,0.0f,  1.49f, 0.54f,  -2560,0.162f,   -613, 0.088f,  79.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS City{ 100,            -1000 ,  -800,0.0f,  1.49f, 0.67f,  -2273,0.007f,  -2217, 0.011f,  50.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Mountains{ 100,       -1000 , -2500,0.0f,  1.49f, 0.21f,  -2780,0.300f,  -2014, 0.100f,  27.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Quarry{ 100,          -1000 , -1000,0.0f,  1.49f, 0.83f, -10000,0.061f,    500, 0.025f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Plain{ 100,           -1000 , -2000,0.0f,  1.49f, 0.50f,  -2466,0.179f,  -2514, 0.100f,  21.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Parkinglot{ 100,      -1000 ,     0,0.0f,  1.65f, 1.50f,  -1363,0.008f,  -1153, 0.012f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Sewerpipe{ 100,       -1000 , -1000,0.0f,  2.81f, 0.14f,    429,0.014f,    648, 0.021f,  80.0f,  60.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Underwater{ 100,      -1000 , -4000,0.0f,  1.49f, 0.10f,   -449,0.007f,   1700, 0.011f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Smallroom{ 100,       -1000 ,  -600,0.0f,  1.10f, 0.83f,   -400,0.005f,    500, 0.010f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Mediumroom{ 100,      -1000 ,  -600,0.0f,  1.30f, 0.83f,  -1000,0.010f,   -200, 0.020f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Largeroom{ 100,       -1000 ,  -600,0.0f,  1.50f, 0.83f,  -1600,0.020f,  -1000, 0.040f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Mediumhall{ 100,      -1000 ,  -600,0.0f,  1.80f, 0.70f,  -1300,0.015f,   -800, 0.030f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Largehall{ 100,       -1000 ,  -600,0.0f,  1.80f, 0.70f,  -2000,0.030f,  -1400, 0.060f, 100.0f, 100.0f, 5000.0f };
		static constexpr XAUDIO2FX_REVERB_I3DL2_PARAMETERS Plate{ 100,           -1000 ,  -200,0.0f,  1.30f, 0.90f,      0,0.002f,      0, 0.010f, 100.0f,  75.0f, 5000.0f };
	};

	/*
		Default constructor, don't have a loaded sound.
	 */
	Sound();
	Sound(const Sound&) = delete;

	/*
		Constructor that load a file if the path is found.
	 */
	Sound(const std::string path);

	/*
		Constructor that set if the sound can be played multiple times together load a file if the path is found.
	 */
	Sound(ST simultaneous_Tracks, const std::string path);

	/*
		Sound destructor.
	 */
	~Sound();



	/*
	 * Load a sound file.
	 *
	 * @param path to the file.
	 * @return bool : if successfully loaded.
	 */
	bool loadFromFile(const std::filesystem::path);



	/*
		Play the file loaded.
	 */
	void play();

	/*
		Resume paused instances of this sound.
	 */
	void resume();

	/*
		Pause all instances of this sound.
	 */
	void pause();

	/*
		Stop all instances of this sound.
	 */
	void stop();



	/*
	 * Change Mixer output.
	 *
	 * @param name of the mixer.
	 * @param
	 * @param See :
	 * @param Audio_System::addSubMixer function.
	 */
	void setMixer(std::string);

	/*
	 * Give the Mixer name.
	 *
	 * @return string : Mixer name.
	 */
	const std::string getMixer() const { return m_Mixer; }



	/*
	 * Set sound volume.
	 *
	 * @param volume to set in percentage (min : 0.f, Max : 100.f).
	 */
	void setVolume(float volume);

	/*
	 * Give the sound volume.
	 *
	 * @return float : volume in percentage.
	 */
	const float getVolume() const { return m_volume; };



	/*
	 * Set if the sound replay a the end of it.
	 *
	 * @param true = replay.
	 */
	void setLooping(bool loop) { m_loop = true; }

	/*
	 * Give if the sound loop.
	 *
	 * @return bool : if the sound loop.
	 */
	const bool isLooping() const { return m_loop; }



	/*
	 * Set the sound speed.
	 *
	 * @param speed to set in percentage (default = 100.f, min : 0.1f, Max : 200.f).
	 */
	void setSpeed(float speed);

	/*
	 * Give the sound speed.
	 *
	 * @return float : speed in percentage.
	 */
	const float getSpeed() const { return m_speed; }



	/*
	 * Give the sound duration.
	 *
	 * @return doule : duration (seconds).
	 */
	const double getAudioDuration();

	/*
	 * Give the sound current time.
	 *
	 * @return doule : current time (seconds), if none lauched (return 0), if multiple lauched return value of the first instance.
	 */
	const double getAudioTime();

	/*
	 * Set the sound time position to given time.
	 *
	 * @param Time as chrono duration.
	 * @param Exemple : sound.setAudioTime(std::chrono::seconds(20));
	 */
	template <class _Rep, class _Period>
	void setAudioTime(const std::chrono::duration<_Rep, _Period>& _Rel_time) {
		std::chrono::milliseconds time = std::chrono::duration_cast<std::chrono::milliseconds>(_Rel_time);

		if (!m_activeChannelPtrs.empty()) {
			if (m_file) {
				if ((time.count() / 1000.0) > this->getAudioDuration()) {
					m_file->setPlayTime(m_activeChannelPtrs.front(), this->getAudioDuration());
				}
				else if (time.count() <= 0) {
					m_file->setPlayTime(m_activeChannelPtrs.front(), 0.0);
				}
				else {
					m_file->setPlayTime(m_activeChannelPtrs.front(), time.count() / 1000.0);
				}
			}
		}
	}




	/*
	 * Give if a file is linked.
	 *
	 * @return bool : if a file is well linked and readable.
	 */
	bool isLinked() const { return m_loaded; }



	/*
	 * Give the sound statut.
	 *
	 * @return Sound::Statut enumeration.
	 */
	Sound::Statut getStatut();



	/*
	 * Set 3D position.
	 *
	 * @param x positon.
	 * @param y positon.
	 * @param z positon.
	 */
	void setPosition(float x, float y, float z);

	/*
	 * Set 3D position.
	 *
	 * @param x, y and z vector.
	 */
	void setPosition(X3DAUDIO_VECTOR pos);

	/*
	 * Move 3D position.
	 *
	 * @param x positon.
	 * @param y positon.
	 * @param z positon.
	 */
	void movePosition(float x, float y, float z);

	/*
	 * Move 3D position.
	 *
	 * @param x, y and z vector.
	 */
	void movePosition(X3DAUDIO_VECTOR pos);

	/*
	 * Give the emitter position, x, y, z.
	 *
	 * @param T : template with contructor float, float, float.
	 * @param Default = Vector3 (Float).
	 *
	 * @return T : position.
	 */
	template <typename T = X3DAUDIO_VECTOR>
	const T getPosition() {
		if constexpr (std::is_same_v<T, decltype(m_X3DEmitter.Position)>) {
			return m_X3DEmitter.Position;
		}
		else {
			static_assert(std::is_constructible_v<T, float, float, float>, "Sound::getPosition() template invalid.");

			return T(m_X3DEmitter.Position.x, m_X3DEmitter.Position.y, m_X3DEmitter.Position.z);
		}
	}



	/*
	 * Give if the 3D calcule is activated.
	 *
	 * @return true if the 3D calcule is activated.
	 */
	bool is3D() const;

	/*
	 * Enable 3D position.
	 *
	 * @param true to enable.
	 */
	void enable3D(bool b);



	/*
	 * Set the max hearing distance.
	 *
	 * @param new distance.
	 */
	void setMaxDistance(float distance);

	/*
	 * Give the max hearing distance.
	 *
	 * @return the hearing distance.
	 */
	const float getMaxDistance() const;



	/*
	 * Set new reverb parameters.
	 *
	 * @param new reverb parameters.
	 */
	void setReverb(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& rev);

	/*
	 * Set new reverb parameters.
	 *
	 * @param new reverb parameters.
	 */
	void setReverb(const XAUDIO2FX_REVERB_PARAMETERS& rev);



	/*
	 * Get dsp information.
	 *
	 * @return dsp information.
	 */
	const X3DAUDIO_DSP_SETTINGS& getDSPInfos() const;

private:
	std::string m_Mixer{ "master" };
	std::mutex m_ChannelList_mutex;
	std::vector<std::reference_wrapper<class Channel>> m_activeChannelPtrs;
	bool m_loaded{ false };
	float m_volume{ 100.f };
	bool m_loop{ false };
	float m_speed{ 100.f };
	bool m_is3D{ false };
	Simultaneous_Tracks m_st{ ST::System_SoundLimit };
	std::shared_ptr<FileProcessing> m_file;

	// 3D
	std::variant<std::reference_wrapper<const XAUDIO2FX_REVERB_PARAMETERS>, std::reference_wrapper<const XAUDIO2FX_REVERB_I3DL2_PARAMETERS>> m_Reverb{ Reverb::Default };
	X3DAUDIO_EMITTER m_X3DEmitter = { 0 };
	X3DAUDIO_CONE m_emitterCone;
	std::array<X3DAUDIO_DISTANCE_CURVE_POINT, 10> m_volumePoints{};
	std::array<X3DAUDIO_DISTANCE_CURVE_POINT, 10> m_reverbPoints{};
	X3DAUDIO_DISTANCE_CURVE m_volumeCurve;
	X3DAUDIO_DISTANCE_CURVE m_reverbCurve;

	std::unique_ptr<float[]> m_matrix;
	X3DAUDIO_DSP_SETTINGS m_DSPSettings = { 0 };

#ifdef _AUDIO_ADMIN_
public:

#pragma region internal_use_admin

	/*
	 * Add a channel to the active channel List.
	 *
	 * @param Channel reference.
	 */
	void addChannel(class Channel& channel) {
		std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
		m_activeChannelPtrs.push_back(channel);
	}

	/*
	 * Remove a channel from the active channel List.
	 *
	 * @param Channel reference.
	 */
	void removeChannel(class Channel& channel) {
		std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
		auto it = std::find_if(std::begin(m_activeChannelPtrs), std::end(m_activeChannelPtrs), [&channel](Channel& _channel) {return &channel == &_channel; });
		if (it != std::end(m_activeChannelPtrs)) {
			m_activeChannelPtrs.erase(it);
		}
	}



	/*
	 * Get reverb parameters.
	 *
	 * @return reverb parameters.
	 */
	std::variant<std::reference_wrapper<const XAUDIO2FX_REVERB_PARAMETERS>, std::reference_wrapper<const XAUDIO2FX_REVERB_I3DL2_PARAMETERS>>& getReverb() {
		return m_Reverb;
	}



	/*
	 * Get FileProcessing class.
	 *
	 * @return FileProcessing pointer.
	 */
	std::shared_ptr<FileProcessing> getFileProcessing() {
		return m_file;
	}



	/*
	 * Get X3DAUDIO_EMITTER struct.
	 *
	 * @return X3DAUDIO_EMITTER struct.
	 */
	X3DAUDIO_EMITTER& getEmitter() {
		return m_X3DEmitter;
	}



	/*
	 * Get the list of channels use by this sound.
	 *
	 * @return list of channels.
	 */
	std::vector<std::reference_wrapper<class Channel>> getActiveChannelPtrs() {
		return m_activeChannelPtrs;
	}



	/*
	 * Get mutex for getActiveChannelPtrs.
	 *
	 * @return std::mutex.
	 */
	std::mutex& channelList_mutex() {
		return m_ChannelList_mutex;
	}

#pragma endregion internal_use_admin

#pragma region user_admin_access

	/*
	 * Get dsp information.
	 *
	 * @return dsp information.
	 */
	X3DAUDIO_DSP_SETTINGS& getDSPInfos() {
		return m_DSPSettings;
	}

#pragma endregion user_admin_access

#endif // _AUDIO_ADMIN_

};
