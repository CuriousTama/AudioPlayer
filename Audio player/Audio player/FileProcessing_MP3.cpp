#define MINIMP3_IMPLEMENTATION
#define _AUDIO_ADMIN_
#include "FileProcessing_MP3.h"
#include "Sound.h"
#include "Channel.h"

FileProcessing_MP3::~FileProcessing_MP3()
{
	for (auto& current : m_Currents_streams) {
		mp3dec_ex_close(&current.second.mp3d);
	}
}

const UINT32 FileProcessing_MP3::get_nBytes(Channel& channel) const
{
	if (m_Currents_streams.contains(&channel)) {
		return m_Currents_streams.find(&channel)->second.m_NumBytes;
	}

	return m_NumBytes;
}

const BYTE* const FileProcessing_MP3::get_pData(Channel& channel) const
{
	if (m_Currents_streams.contains(&channel)) {
		return m_Currents_streams.find(&channel)->second.m_pData.get();
	}

	return m_pData.get();
}

bool FileProcessing_MP3::load(const std::string path)
{
	m_path = path;
	mp3dec_ex_t mp3d{ 0 };
	if (mp3dec_ex_open(&mp3d, m_path.c_str(), MP3D_SEEK_TO_SAMPLE) < 0) {
		std::string error("[error] : File does not appear to be an Mp3 file : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
		throw std::exception(error.c_str());
	}

	m_format.wFormatTag = WAVE_FORMAT_PCM;
	m_format.nChannels = mp3d.info.channels;
	m_format.nSamplesPerSec = mp3d.info.hz;
	m_format.wBitsPerSample = 16;
	m_format.nBlockAlign = m_format.nChannels * (m_format.wBitsPerSample / 8);
	m_format.nAvgBytesPerSec = m_format.nChannels * m_format.nSamplesPerSec * (m_format.wBitsPerSample / 8);
	m_format.cbSize = 0;

	if (m_format.nSamplesPerSec == 0) {
		mp3dec_ex_close(&mp3d);
		return false;
	}

	m_AudioDuration = (mp3d.samples * 2 / mp3d.info.channels) * sizeof(mp3d_sample_t) / static_cast<double>(m_format.nSamplesPerSec) / 4.0;
	mp3dec_ex_close(&mp3d);

	return true;
}

void FileProcessing_MP3::play(Channel& channel)
{
	S_Mp3_stream& Current = m_Currents_streams[&channel];

	if (mp3dec_ex_open(&Current.mp3d, m_path.c_str(), MP3D_SEEK_TO_SAMPLE) < 0) {
		std::string error("[error] : File does not appear to be an Mp3 file : " + m_path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
		throw std::exception(error.c_str());
	}

	Current.m_pData = std::make_unique<BYTE[]>(read * sizeof(mp3d_sample_t));
	size_t readed = mp3dec_ex_read(&Current.mp3d, (mp3d_sample_t*)Current.m_pData.get(), read);

	if (Current.mp3d.last_error == 0) {
		Current.m_NumBytes = read * sizeof(mp3d_sample_t);
	}
	else {
		Current.m_NumBytes = 0;
	}
}

bool FileProcessing_MP3::OnBufferEnd(Channel& channel)
{
	if (!m_Currents_streams.contains(&channel)) {
		removeStream(channel);
		return true;
	}

	S_Mp3_stream& Current = m_Currents_streams.find(&channel)->second;

	if (!Current.m_end && !channel.isForcedStop()) {
		size_t readed = mp3dec_ex_read(&Current.mp3d, (mp3d_sample_t*)Current.m_pData.get(), read);

		if (readed > 0) {
			Current.m_NumBytes = static_cast<UINT32>(readed * sizeof(mp3d_sample_t));
			channel.getAudioBuffer().AudioBytes = Current.m_NumBytes;

			std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());
			channel.getSourceVoice()->SubmitSourceBuffer(&channel.getAudioBuffer(), nullptr);
		}
		else if (channel.getSound().isLooping()) {
			mp3dec_ex_seek(&Current.mp3d, 0);
			this->OnBufferEnd(channel);
		}
		else {
			mp3dec_ex_seek(&Current.mp3d, 0);
			Current.m_end = true;
			removeStream(channel);
			return true;
		}
	}
	else {
		mp3dec_ex_seek(&Current.mp3d, 0);
		removeStream(channel);
		return true;
	}


	return false;
}

void FileProcessing_MP3::removeStream(Channel& channel)
{
	m_Currents_streams.erase(&channel);
}

double FileProcessing_MP3::fileDuration()
{
	return m_AudioDuration;
}

double FileProcessing_MP3::currentPlayTime(Channel& channel)
{
	if (!m_Currents_streams.contains(&channel)) {
		return 0.0;
	}

	S_Mp3_stream& Current = m_Currents_streams.find(&channel)->second;

	if (m_format.nSamplesPerSec != 0 && Current.mp3d.info.channels != 0) {
		return Current.mp3d.cur_sample * (2 / Current.mp3d.info.channels) / sizeof(mp3d_sample_t) / static_cast<double>(m_format.nSamplesPerSec);
	}

	return 0.0;
}

void FileProcessing_MP3::setPlayTime(Channel& channel, double time)
{
	if (!m_Currents_streams.contains(&channel)) {
		return;
	}

	S_Mp3_stream& Current = m_Currents_streams.find(&channel)->second;

	if (m_format.nSamplesPerSec != 0 && Current.mp3d.info.channels != 0) {
		mp3dec_ex_seek(&Current.mp3d, static_cast<uint64_t>((m_format.nSamplesPerSec * 2.0) * time / (2 / Current.mp3d.info.channels)));
	}
}
