#define _AUDIO_ADMIN_
#include "Audio_System.h"
#include "Sound.h"
#include "Channel.h"
#include <iostream>

Audio_System::~Audio_System()
{
	m_pEngine->StopEngine();
	m_pMaster->DestroyVoice();
	m_activeChannelPtrs.clear();
	m_buffers.clear();

	CoUninitialize();

	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);
}

bool Audio_System::addSubMixer(std::string MixerName)
{
	std::transform(std::begin(MixerName), std::end(MixerName), std::begin(MixerName), tolower);

	if (!m_SubVoices.contains(MixerName) && MixerName != "MASTER") {
		m_pEngine->CreateSubmixVoice(&m_SubVoices[MixerName], 2, 44100);
		return true;
	}

	return false;
}

bool Audio_System::removeSubMixer(std::string MixerName)
{
	std::transform(std::begin(MixerName), std::end(MixerName), std::begin(MixerName), tolower);

	if (m_SubVoices.contains(MixerName) && MixerName != "MASTER") {
		m_SubVoices[MixerName]->DestroyVoice();
		m_SubVoices.erase(MixerName);

		while (!m_activeChannelPtrs[MixerName].empty()) {
			for (auto& c : m_activeChannelPtrs[MixerName]) {
				removeChannel_(*c);
			}
		}

		m_activeChannelPtrs.erase(MixerName);
		return true;
	}

	return false;
}

void Audio_System::setMasterVolume(float volume)
{
	m_pMaster->SetVolume(std::clamp(volume / 100.f, 0.f, 1.f));
}

void Audio_System::setSubMixerVolume(std::string MixerName, float volume)
{
	volume = std::clamp(volume, 0.f, 100.f);
	std::transform(std::begin(MixerName), std::end(MixerName), std::begin(MixerName), tolower);

	if (m_SubVoices.contains(MixerName)) {
		m_SubVoices[MixerName]->SetVolume(volume / 100.f);
	}
}


const float Audio_System::getMasterVolume()
{
	float volume = 0.f;
	m_pMaster->GetVolume(&volume);

	return volume * 100.f;
}

void Audio_System::setListenerPosition(float x, float y, float z)
{
	m_X3DListener.Position = X3DAUDIO_VECTOR(x, y, z);
	recalculate3D();
}

void Audio_System::setListenerPosition(X3DAUDIO_VECTOR pos)
{
	m_X3DListener.Position = pos;
	recalculate3D();
}

void Audio_System::moveListenerPosition(float x, float y, float z)
{
	m_X3DListener.Position.x += x;
	m_X3DListener.Position.y += y;
	m_X3DListener.Position.z += z;
	recalculate3D();
}

void Audio_System::moveListenerPosition(X3DAUDIO_VECTOR pos)
{
	m_X3DListener.Position.x += pos.x;
	m_X3DListener.Position.y += pos.y;
	m_X3DListener.Position.z += pos.z;
	recalculate3D();
}

void Audio_System::setListenerForward(float x, float y, float z)
{
	m_X3DListener.OrientFront.x = x;
	m_X3DListener.OrientFront.y = y;
	m_X3DListener.OrientFront.z = z;
	recalculate3D();
}

void Audio_System::setListenerForward(X3DAUDIO_VECTOR forward)
{
	m_X3DListener.OrientFront = forward;
	recalculate3D();
}

void Audio_System::setListenerOrientation(X3DAUDIO_VECTOR forward, X3DAUDIO_VECTOR TopOfHead)
{
	m_X3DListener.OrientFront = forward;
	m_X3DListener.OrientTop = TopOfHead;
	recalculate3D();
}

void Audio_System::setListenerOrientation(float f_x, float f_y, float f_z, X3DAUDIO_VECTOR TopOfHead)
{
	m_X3DListener.OrientFront.x = f_x;
	m_X3DListener.OrientFront.y = f_y;
	m_X3DListener.OrientFront.z = f_z;
	m_X3DListener.OrientTop = TopOfHead;
	recalculate3D();
}

void Audio_System::setListenerOrientation(X3DAUDIO_VECTOR forward, float t_x, float t_y, float t_z)
{
	m_X3DListener.OrientFront = forward;
	m_X3DListener.OrientTop.x = t_x;
	m_X3DListener.OrientTop.y = t_y;
	m_X3DListener.OrientTop.z = t_z;
	recalculate3D();
}

void Audio_System::setListenerOrientation(float f_x, float f_y, float f_z, float t_x, float t_y, float t_z)
{
	m_X3DListener.OrientFront.x = f_x;
	m_X3DListener.OrientFront.y = f_y;
	m_X3DListener.OrientFront.z = f_z;
	m_X3DListener.OrientTop.x = t_x;
	m_X3DListener.OrientTop.y = t_y;
	m_X3DListener.OrientTop.z = t_z;
	recalculate3D();
}

void Audio_System::setDefaultMaxDistance(float maxDistance)
{
	m_DefaultMaxDistance = maxDistance;
}

const float Audio_System::getDefaultMaxDistance()
{
	return m_DefaultMaxDistance;
}

const float Audio_System::getSubMixerVolume(std::string MixerName)
{
	float volume = 0.f;
	std::transform(std::begin(MixerName), std::end(MixerName), std::begin(MixerName), tolower);

	if (m_SubVoices.contains(MixerName)) {
		m_SubVoices[MixerName]->GetVolume(&volume);
	}

	return volume * 100.f;
}

void Audio_System::removeChannel_(Channel& channel)
{
	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);

	const std::string Target_Mixer = channel.getMixerName();

	auto it = std::find_if(std::begin(m_activeChannelPtrs[Target_Mixer]), std::end(m_activeChannelPtrs[Target_Mixer]),
		[&channel](const std::unique_ptr<Channel>& pChan) {
			return &channel == pChan.get();
		});

	if (it != std::end(m_activeChannelPtrs[Target_Mixer])) {
		m_activeChannelPtrs[Target_Mixer].erase(it);
	}
}

void Audio_System::play(Sound& sound)
{
	std::lock_guard<std::mutex> lock(m_ChannelList_mutex);

	std::string Target_Mixer(sound.getMixer());

	if (Target_Mixer != "MASTER" && !m_SubVoices.contains(Target_Mixer)) {
		sound.setMixer("MASTER");
		Target_Mixer = sound.getMixer();
	}

	if (Target_Mixer == "MASTER" || m_SubVoices.contains(Target_Mixer)) {
		if (m_activeChannelPtrs[Target_Mixer].size() < static_cast<size_t>(m_MaxChannelsPerMixer))
		{
			m_activeChannelPtrs[Target_Mixer].emplace_back(std::make_unique<Channel>(Target_Mixer, sound));
			try {
				m_activeChannelPtrs[Target_Mixer].back()->play();
			}
			catch (...) {
				m_activeChannelPtrs[Target_Mixer].pop_back();
				throw;
			}
		}
	}
}


void Audio_System::recalculate3D()
{
	for (auto& [mixer, Vec_channel] : m_activeChannelPtrs) {
		for (auto& channel : Vec_channel) {
			channel->calculate3D();
		}
	}
}


Audio_System::constructor::constructor()
{
	std::ignore = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	XAudio2Create(&Audio_System::m_pEngine);
	Audio_System::m_pEngine->CreateMasteringVoice(&Audio_System::m_pMaster, 2, 44100);

	DWORD dwChannelMask;
	Audio_System::m_pMaster->GetChannelMask(&dwChannelMask);
	X3DAudioInitialize(dwChannelMask, X3DAUDIO_SPEED_OF_SOUND, Audio_System::m_X3DInstance);

	m_X3DListener.OrientFront.z = 1.0f;
	m_X3DListener.OrientTop.y = 1.0f;
	m_X3DListener.pCone = nullptr;
}