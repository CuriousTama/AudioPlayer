#define _AUDIO_ADMIN_
#include "Sound.h"

#include "FileProcessing_WAV.h"
#include "FileProcessing_OGG.h"
#include "FileProcessing_MP3.h"

#include "Channel.h"


namespace {
	void SetupEnvironment(
		std::array<X3DAUDIO_DISTANCE_CURVE_POINT, 10>& m_volumePoints,
		X3DAUDIO_DISTANCE_CURVE& m_volumeCurve,
		std::array<X3DAUDIO_DISTANCE_CURVE_POINT, 10>& m_reverbPoints,
		X3DAUDIO_DISTANCE_CURVE& m_reverbCurve,
		X3DAUDIO_EMITTER& m_X3DEmitter,
		X3DAUDIO_CONE& m_emitterCone)
	{
		//memset(&m_X3DListener, 0, sizeof(X3DAUDIO_LISTENER));
		memset(&m_X3DEmitter, 0, sizeof(X3DAUDIO_EMITTER));

		//Listener is facing the bottom of the screen
		/*Audio_System::m_X3DListener.OrientFront.y = -1.0f;
		Audio_System::m_X3DListener.OrientTop.z = 1.0f;*/
		//m_listenerAngle = 0.0f;

		//Emitter is facing the top of the screen
		//m_X3DEmitter.OrientFront.y = -1.0f;
		//m_X3DEmitter.OrientTop.z = 1.0f;
		//m_emitterAngle = X3DAUDIO_PI;

		//Audio in use is in mono
		m_X3DEmitter.ChannelCount = 1;

		//Volume attenuation curve
		m_volumePoints[0].Distance = 0.0f;
		m_volumePoints[0].DSPSetting = 1.0f;
		m_volumePoints[1].Distance = 0.2f;
		m_volumePoints[1].DSPSetting = 1.0f;
		m_volumePoints[2].Distance = 0.3f;
		m_volumePoints[2].DSPSetting = 0.5f;
		m_volumePoints[3].Distance = 0.4f;
		m_volumePoints[3].DSPSetting = 0.35f;
		m_volumePoints[4].Distance = 0.5f;
		m_volumePoints[4].DSPSetting = 0.23f;
		m_volumePoints[5].Distance = 0.6f;
		m_volumePoints[5].DSPSetting = 0.16f;
		m_volumePoints[6].Distance = 0.7f;
		m_volumePoints[6].DSPSetting = 0.1f;
		m_volumePoints[7].Distance = 0.8f;
		m_volumePoints[7].DSPSetting = 0.06f;
		m_volumePoints[8].Distance = 0.9f;
		m_volumePoints[8].DSPSetting = 0.04f;
		m_volumePoints[9].Distance = 1.0f;
		m_volumePoints[9].DSPSetting = 0.0f;
		m_volumeCurve.PointCount = 10;
		m_volumeCurve.pPoints = &m_volumePoints[0];

		//Reverb attenuation curve
		m_reverbPoints[0].Distance = 0.0f;
		m_reverbPoints[0].DSPSetting = 0.7f;
		m_reverbPoints[1].Distance = 0.2f;
		m_reverbPoints[1].DSPSetting = 0.78f;
		m_reverbPoints[2].Distance = 0.3f;
		m_reverbPoints[2].DSPSetting = 0.85f;
		m_reverbPoints[3].Distance = 0.4f;
		m_reverbPoints[3].DSPSetting = 1.0f;
		m_reverbPoints[4].Distance = 0.5f;
		m_reverbPoints[4].DSPSetting = 1.0f;
		m_reverbPoints[5].Distance = 0.6f;
		m_reverbPoints[5].DSPSetting = 0.6f;
		m_reverbPoints[6].Distance = 0.7f;
		m_reverbPoints[6].DSPSetting = 0.4f;
		m_reverbPoints[7].Distance = 0.8f;
		m_reverbPoints[7].DSPSetting = 0.25f;
		m_reverbPoints[8].Distance = 0.9f;
		m_reverbPoints[8].DSPSetting = 0.11f;
		m_reverbPoints[9].Distance = 1.0f;
		m_reverbPoints[9].DSPSetting = 0.0f;
		m_reverbCurve.PointCount = 10;
		m_reverbCurve.pPoints = &m_reverbPoints[0];

		//Add the curves to every emitter
		m_X3DEmitter.pVolumeCurve = &m_volumeCurve;
		m_X3DEmitter.pReverbCurve = &m_reverbCurve;

		//Emitter cone (only for the first "mode")
		m_emitterCone.InnerAngle = X3DAUDIO_2PI;
		m_emitterCone.OuterAngle = X3DAUDIO_PI;
		m_emitterCone.InnerVolume = 1.0f;
		m_emitterCone.OuterVolume = 0.0f;
		m_emitterCone.InnerReverb = 1.0f;
		m_emitterCone.OuterReverb = 0.0f;
		m_X3DEmitter.pCone = &m_emitterCone;

		//Set how much distance influences the sound
		m_X3DEmitter.CurveDistanceScaler = Audio_System::getDefaultMaxDistance();

		//Start the listener and emitter in the middle of the screen
		//auto rect = m_deviceResources->GetOutputSize();
		//m_X3DListener.Position = X3DAUDIO_VECTOR(float(0 / 2), float(0 / 2), 0);
		m_X3DEmitter.Position = X3DAUDIO_VECTOR(0, 0, 0);
	}

}


Sound::Sound()
{
	SetupEnvironment(m_volumePoints, m_volumeCurve, m_reverbPoints, m_reverbCurve, m_X3DEmitter, m_emitterCone);

	m_matrix = std::make_unique<float[]>(12);
	m_DSPSettings.SrcChannelCount = 1;
	m_DSPSettings.DstChannelCount = Audio_System::getVoiceDetails().InputChannels;
	m_DSPSettings.pMatrixCoefficients = m_matrix.get();
}

Sound::Sound(const std::string path) : Sound()
{
	this->loadFromFile(path);
}

Sound::Sound(ST simultaneous_Tracks, const std::string path) : Sound(path)
{
	m_st = simultaneous_Tracks;
}

Sound::~Sound()
{
	{
		std::lock_guard<std::mutex> lock(m_ChannelList_mutex);

		std::for_each(std::begin(m_activeChannelPtrs), std::end(m_activeChannelPtrs), [](Channel& channel) {
			channel.Stop(); });
	}

	bool allChannelsDeactivated = false;
	do
	{
		std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
		allChannelsDeactivated = m_activeChannelPtrs.empty();
	} while (!allChannelsDeactivated);

	this->m_file.reset();
	Audio_System::updateBuffersLife();

	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
}

bool Sound::loadFromFile(const std::filesystem::path path)
{
	m_loaded = false;

	if (std::filesystem::exists(path)) {
		auto it = Audio_System::getBuffer().find(path);

		this->stop();
		this->m_file.reset();

		if (it == std::end(Audio_System::getBuffer())) {

			auto extention = path.extension();

			if (extention == ".ogg") {
				Audio_System::getBuffer()[path] = std::make_shared<FileProcessing_OGG>();
				this->m_file = Audio_System::getBuffer()[path];
			}
			else if (extention == ".wav") {
				Audio_System::getBuffer()[path] = std::make_shared<FileProcessing_WAV>();
				this->m_file = Audio_System::getBuffer()[path];
			}
			else if (extention == ".mp3") {
				Audio_System::getBuffer()[path] = std::make_shared<FileProcessing_MP3>();
				this->m_file = Audio_System::getBuffer()[path];
			}

			if (this->m_file) {
				m_loaded = this->m_file->load(path.string());
			}
		}
		else {
			this->m_file = it->second;
			m_loaded = true;
		}

		Audio_System::updateBuffersLife();
	}

	if (!m_loaded) {
		std::string error("[error] : Failed to open : " + path.string() + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
		throw std::exception(error.c_str());
	}

	return m_loaded;
}

void Sound::play()
{
	if (m_loaded) {
		m_ChannelList_mutex.lock();
		if (m_activeChannelPtrs.empty() || m_st != Simultaneous_Tracks::One) {
			m_ChannelList_mutex.unlock();
			Audio_System::play(*this);
		}
		else {
			m_ChannelList_mutex.unlock();
		}
	}
}

void Sound::resume()
{
	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
	std::for_each(std::begin(m_activeChannelPtrs), std::end(m_activeChannelPtrs), [](Channel& channel) {
		channel.resume(); });
}

void Sound::pause()
{
	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
	std::for_each(std::begin(m_activeChannelPtrs), std::end(m_activeChannelPtrs), [](Channel& channel) {
		channel.pause(); });
}

void Sound::stop()
{
	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
	std::for_each(std::begin(m_activeChannelPtrs), std::end(m_activeChannelPtrs), [](Channel& channel) {
		channel.Stop(); });
}

void Sound::setMixer(std::string MixerName)
{
	std::transform(std::begin(MixerName), std::end(MixerName), std::begin(MixerName), tolower);
	m_Mixer = MixerName;
}

void Sound::setVolume(float volume)
{
	m_volume = std::clamp(volume, 0.f, 100.f);

	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
	std::for_each(std::begin(m_activeChannelPtrs), std::end(m_activeChannelPtrs), [this](Channel& channel) {
		channel.setVolume(m_volume); });
}

void Sound::setSpeed(float speed)
{
	m_speed = std::clamp(speed, 0.1f, 200.f);

	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
	std::for_each(std::begin(m_activeChannelPtrs), std::end(m_activeChannelPtrs), [this](Channel& channel) {
		channel.setSpeed(m_speed); });
}

const double Sound::getAudioDuration()
{
	if (m_file) {
		return m_file->fileDuration();
	}

	return 0.0;
}

const double Sound::getAudioTime()
{
	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
	if (!m_activeChannelPtrs.empty()) {
		if (m_file) {
			return m_file->currentPlayTime(m_activeChannelPtrs.front());
		}
	}

	return 0.0;
}

void Sound::setPosition(float x, float y, float z)
{
	m_X3DEmitter.Position = X3DAUDIO_VECTOR(x, y, z);
	this->enable3D(true);
}

void Sound::setPosition(X3DAUDIO_VECTOR pos)
{
	m_X3DEmitter.Position = pos;
	this->enable3D(true);
}

void Sound::movePosition(float x, float y, float z)
{
	m_X3DEmitter.Position.x += x;
	m_X3DEmitter.Position.y += y;
	m_X3DEmitter.Position.z += z;
	this->enable3D(true);
}

void Sound::movePosition(X3DAUDIO_VECTOR pos)
{
	m_X3DEmitter.Position.x += pos.x;
	m_X3DEmitter.Position.y += pos.y;
	m_X3DEmitter.Position.z += pos.z;
	this->enable3D(true);
}

bool Sound::is3D() const
{
	return m_is3D;
}

void Sound::enable3D(bool b)
{
	m_is3D = b;

	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
	for (auto& channel : m_activeChannelPtrs) {
		channel.get().enable3D();
	}
}

void Sound::setMaxDistance(float distance)
{
	m_X3DEmitter.CurveDistanceScaler = distance;
	this->enable3D(m_is3D);
}

const float Sound::getMaxDistance() const
{
	return m_X3DEmitter.CurveDistanceScaler;
}

void Sound::setReverb(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& rev)
{
	m_Reverb = rev;

	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
	for (auto& channel : m_activeChannelPtrs) {
		channel.get().setReverb(rev);
	}
}

void Sound::setReverb(const XAUDIO2FX_REVERB_PARAMETERS& rev)
{
	m_Reverb = rev;

	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
	for (auto& channel : m_activeChannelPtrs) {
		channel.get().setReverb(rev);
	}
}

const X3DAUDIO_DSP_SETTINGS& Sound::getDSPInfos() const
{
	return m_DSPSettings;
}

Sound::Statut Sound::getStatut()
{
	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);

	if (!m_loaded) {
		return Sound::Statut::not_ready;
	}
	else if (m_activeChannelPtrs.empty()) {
		return Sound::Statut::stopped;
	}
	else {
		auto num_paused = std::count_if(std::begin(m_activeChannelPtrs), std::end(m_activeChannelPtrs), [](Channel& channel) {
			return channel.getSpeed() <= 0.05f; });

		if (num_paused == 0) {
			return Sound::Statut::playing;
		}
		else if (num_paused == m_activeChannelPtrs.size()) {
			return Sound::Statut::paused;
		}
		else {
			return Sound::Statut::partially_paused;
		}
	}
}
