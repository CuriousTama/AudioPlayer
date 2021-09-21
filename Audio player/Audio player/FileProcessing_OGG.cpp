#define _AUDIO_ADMIN_
#include "FileProcessing_OGG.h"
#include "Sound.h"
#include "Channel.h"

#include <assert.h>
#include <iostream>

size_t read(void* buffer, size_t elementSize, size_t elementCount, void* dataSource) {
	assert(elementSize == 1);

	std::ifstream* stream = static_cast<std::ifstream*>(dataSource);
	stream->read(static_cast<char*>(buffer), elementCount);
	const size_t bytesRead = static_cast<size_t>(stream->gcount());
	stream->clear(); // In case we read past EOF
	return bytesRead;
}

int seek(void* dataSource, ogg_int64_t offset, int origin) {
	static const std::vector<std::ios_base::seekdir> seekDirections{ std::ios_base::beg, std::ios_base::cur, std::ios_base::end };

	std::ifstream& stream = *static_cast<std::ifstream*>(dataSource);
	stream.seekg(offset, seekDirections.at(origin));
	stream.clear(); // In case we seeked to EOF
	return 0;
}

long tell(void* dataSource) {
	std::ifstream& stream = *static_cast<std::ifstream*>(dataSource);
	const auto position = stream.tellg();
	assert(position >= 0);
	return static_cast<long>(position);
}

FileProcessing_OGG::~FileProcessing_OGG()
{
	for (auto& current : m_Currents_streams) {
		ov_clear(&current.second.m_vf);
	}

	m_Currents_streams.clear();
	ov_clear(&m_info_vf);
}

const UINT32 FileProcessing_OGG::get_nBytes(Channel& channel) const
{
	if (m_Currents_streams.contains(&channel)) {
		return m_Currents_streams.find(&channel)->second.m_NumBytes;
	}

	return m_NumBytes;
}

const BYTE* const FileProcessing_OGG::get_pData(Channel& channel) const
{
	if (m_Currents_streams.contains(&channel)) {
		return m_Currents_streams.find(&channel)->second.m_pData.get();
	}

	return m_pData.get();
}

bool FileProcessing_OGG::load(const std::string path)
{
	m_info_stream.close();
	m_path = path;

	m_info_stream.open(m_path, std::ios::binary);
	const ov_callbacks callbacks{ read, seek, nullptr, tell };

	if (ov_open_callbacks(&m_info_stream, &m_info_vf, NULL, 0, callbacks) < 0) {
		std::string error("[error] : File does not appear to be an Ogg bitstream : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
		throw std::exception(error.c_str());
	}

	vorbis_info* vi = ov_info(&m_info_vf, -1);

	m_format.wFormatTag = WAVE_FORMAT_PCM;
	m_format.nChannels = vi->channels;
	m_format.nSamplesPerSec = vi->rate;
	m_format.wBitsPerSample = 16;
	m_format.nBlockAlign = m_format.nChannels * (m_format.wBitsPerSample / 8);
	m_format.nAvgBytesPerSec = m_format.nChannels * m_format.nSamplesPerSec * (m_format.wBitsPerSample / 8);
	m_format.cbSize = 0;

	return true;
}

void FileProcessing_OGG::play(Channel& channel)
{
	S_Ogg_stream& Current = m_Currents_streams[&channel];

	Current.m_stream.open(m_path, std::ios::binary);

	const ov_callbacks callbacks{ read, seek, nullptr, tell };

	if (ov_open_callbacks(&Current.m_stream, &Current.m_vf, NULL, 0, callbacks) < 0) {
		std::string error("[error] : Fail to open callbacks OGG\n[File] " __FILE__  "\n[Line] " + std::to_string(__LINE__));
		throw std::exception(error.c_str());
	}

	long ret = ov_read(&Current.m_vf, std::bit_cast<char*, BYTE*>(Current.m_buffer.data()), BufferSize, 0, 2, 1, nullptr);

	Current.m_NumBytes = BufferSize;
	Current.m_pData = std::make_unique<BYTE[]>(Current.m_NumBytes);
	memmove(Current.m_pData.get(), Current.m_buffer.data(), Current.m_NumBytes);
}

bool FileProcessing_OGG::OnBufferEnd(Channel& channel)
{
	if (!m_Currents_streams.contains(&channel) ) {
		removeStream(channel);
		return true;
	}

	S_Ogg_stream& Current = m_Currents_streams.find(&channel)->second;

	if (!Current.m_end && !channel.isForcedStop()) {
		int current_section = 0;

		long ret = ov_read(&Current.m_vf, std::bit_cast<char*, BYTE*>(Current.m_buffer.data()), BufferSize, 0, 2, 1, &current_section);

		if (ret > 0) {
			Current.m_NumBytes = ret;
			channel.getAudioBuffer().AudioBytes = Current.m_NumBytes;
			memmove(Current.m_pData.get(), Current.m_buffer.data(), Current.m_NumBytes);

			std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());
			channel.getSourceVoice()->SubmitSourceBuffer(&channel.getAudioBuffer(), nullptr);
		}
		else if (channel.getSound().isLooping()) {
			ov_time_seek(&Current.m_vf, 0.0);
			this->OnBufferEnd(channel);
			return false;
		}
		else {
			Current.m_end = true;
			ov_time_seek(&Current.m_vf, 0.0);
			removeStream(channel);
			return true;
		}
	}
	else {
		ov_time_seek(&Current.m_vf, 0.0);
		removeStream(channel);
		return true;
	}

	return false;
}

void FileProcessing_OGG::removeStream(Channel& channel)
{
	if (!m_Currents_streams.contains(&channel)) {
		return;
	}

	ov_clear(&m_Currents_streams[&channel].m_vf);
	m_Currents_streams.erase(&channel);
}

double FileProcessing_OGG::fileDuration()
{
	vorbis_info* vi = ov_info(&m_info_vf, -1);
	return ov_time_total(&m_info_vf, -1);
}

double FileProcessing_OGG::currentPlayTime(Channel& channel)
{
	if (!m_Currents_streams.contains(&channel)) {
		return 0.0;
	}

	S_Ogg_stream& Current = m_Currents_streams.find(&channel)->second;
	vorbis_info* vi = ov_info(&Current.m_vf, -1);

	return ov_time_tell(&Current.m_vf);
}

void FileProcessing_OGG::setPlayTime(Channel& channel, double time)
{
	if (!m_Currents_streams.contains(&channel) ) {
		return;
	}

	S_Ogg_stream& Current = m_Currents_streams.find(&channel)->second;
	ov_time_seek(&Current.m_vf, time);
}
