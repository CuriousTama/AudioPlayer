#define _AUDIO_ADMIN_
#include "FileProcessing_WAV.h"
#include "Sound.h"
#include "Channel.h"
#include <fstream>
#include <iostream>

FileProcessing_WAV::FileProcessing_WAV()
{
	m_Reset_data = std::make_unique<BYTE[]>(4);

	for (int i = 0; i < 4; i++)
	{
		m_Reset_data[i] = 1;
	}

	m_Reset_Buffer.AudioBytes = 4;
	m_Reset_Buffer.Flags = XAUDIO2_END_OF_STREAM;
	m_Reset_Buffer.pAudioData = m_Reset_data.get();
}

FileProcessing_WAV::~FileProcessing_WAV()
{
	std::lock_guard<std::mutex> lock(m_TimeChanging_mutex);
}

bool FileProcessing_WAV::load(const std::string path)
{
	int fileSize = 0;
	std::unique_ptr<BYTE[]> pFileIn;

	{
		//The data of a wave file is stored in little endian
		std::ifstream file(path, std::ios::binary);

		if (!file) {
			std::string error("[error] : Failed to open : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
			throw std::exception(error.c_str());
		}

		file.seekg(0, std::ios::end);
		fileSize = static_cast<int>(file.tellg());
		file.seekg(0, std::ios::beg);

		pFileIn = std::make_unique<BYTE[]>(fileSize);
		file.read(std::bit_cast<char*>(pFileIn.get()), fileSize);
	}

	if (*std::bit_cast<const int*>(&pFileIn[0]) != 'FFIR') {
		std::string error("[error] : bad fourCC : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
		throw std::exception(error.c_str());
	}

	else if (fileSize <= 44) { // 44 is start of data chunk
		std::string error("[error] : file too small : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
		throw std::exception(error.c_str());
	}

	else if (*std::bit_cast<const int*>(&pFileIn[8]) != 'EVAW') {
		std::string error("[error] : format not WAVE : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
		throw std::exception(error.c_str());
	}

	//look for 'fmt ' chunk id
	//WAVEFORMATEX format;
	bool bFilledFormat = false;
	for (size_t i = 12; i < static_cast<size_t>(fileSize); ) {
		if (*std::bit_cast<const int*>(&pFileIn[i]) == ' tmf') {
			memcpy(&m_format, &pFileIn[i + 8], sizeof(m_format));
			bFilledFormat = true;
			break;
		}
		// chunk size + size entry size + chunk id entry size + word padding
		i += (*std::bit_cast<const int*>(&pFileIn[i + 4]) + 9) & 0xFFFFFFFE;
	}

	if (!bFilledFormat) {
		std::string error("[error] : fmt chunk not found : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
		throw std::exception(error.c_str());
	}

	// compare format with sound system format
	{
		if (m_format.wFormatTag != WAVE_FORMAT_PCM) {
			std::string error("[error] : bad wave format (wFormatTag) : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
			throw std::exception(error.c_str());
		}

		else if (m_format.nChannels != 1 && m_format.nChannels != 2) {
			std::string error("[error] : bad wave format (nChannels) : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
			throw std::exception(error.c_str());
		}

		else if (m_format.wBitsPerSample != 8 && m_format.wBitsPerSample != 16 && m_format.wBitsPerSample != 24 && m_format.wBitsPerSample != 32) {
			std::string error("[error] : bad wave format (wBitsPerSample) : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
			throw std::exception(error.c_str());
		}

		else if (m_format.nSamplesPerSec <= 0) {
			std::string error("[error] : bad wave format (nSamplesPerSec) : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
			throw std::exception(error.c_str());
		}


		else if (m_format.nBlockAlign != m_format.nChannels * (m_format.wBitsPerSample / 8)) {
			std::string error("[error] : bad wave format (nBlockAlign) : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
			throw std::exception(error.c_str());
		}

		else if (m_format.nAvgBytesPerSec != m_format.nChannels * m_format.nSamplesPerSec * (m_format.wBitsPerSample / 8)) {
			std::string error("[error] : bad wave format (nAvgBytesPerSec) : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
			throw std::exception(error.c_str());
		}

		m_format.cbSize = 0;
	}

	//look for 'data' chunk id
	for (size_t i = 12; i < static_cast<size_t>(fileSize);) {
		const int chunkSize = *std::bit_cast<const int*>(&pFileIn[i + 4]);
		if (*std::bit_cast<const int*>(&pFileIn[i]) == 'atad') {
			m_pData = std::make_unique<BYTE[]>(chunkSize);
			m_NumBytes = chunkSize;
			memcpy(m_pData.get(), &pFileIn[i + 8], m_NumBytes);

			return true;
		}
		// chunk size + size entry size + chunk id entry size + word padding
		i += (chunkSize + 9) & 0xFFFFFFFE;
	}

	std::string error("[error] : data chunk not found : " + path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
	throw std::exception(error.c_str());
}

bool FileProcessing_WAV::OnBufferEnd(Channel& channel)
{
	std::lock_guard<std::mutex> lock(m_TimeChanging_mutex);
	auto it = std::find_if(std::begin(m_Timechanging), std::end(m_Timechanging), [&channel](Channel& _channel) { return &channel == &_channel; });
	bool TimeChange = it != std::end(m_Timechanging);

	if (channel.getSound().isLooping() && !channel.isForcedStop()) {
		std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());

		channel.getAudioBuffer().PlayBegin = 0;
		channel.getSourceVoice()->SubmitSourceBuffer(&m_Reset_Buffer, nullptr);
		channel.getSourceVoice()->SubmitSourceBuffer(&channel.getAudioBuffer(), nullptr);
	}
	else if (TimeChange) {
		m_Timechanging.erase(it);
	}
	else {
		return true;
	}

	return false;
}

double FileProcessing_WAV::fileDuration()
{
	if (m_format.nSamplesPerSec != 0) {
		return (m_NumBytes * 2 / m_format.nChannels) / static_cast<double>(m_format.nSamplesPerSec) / 4.0;
	}

	return 0.0;
}

double FileProcessing_WAV::currentPlayTime(Channel& channel)
{
	if (m_format.nSamplesPerSec != 0) {
		std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());

		XAUDIO2_VOICE_STATE tmp;
		channel.getSourceVoice()->GetState(&tmp);

		return (channel.getAudioBuffer().PlayBegin + tmp.SamplesPlayed) / static_cast<double>(m_format.nSamplesPerSec);
	}

	return 0.0;
}

void FileProcessing_WAV::setPlayTime(Channel& channel, double time)
{
	{
		std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());
		std::lock_guard<std::mutex> lock2(m_TimeChanging_mutex);

		m_Timechanging.push_back(channel);
		channel.getSourceVoice()->Stop();
		channel.getSourceVoice()->FlushSourceBuffers();
	}

	bool waitingReset = false;
	do
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		std::lock_guard<std::mutex> lock(m_TimeChanging_mutex);
		waitingReset = std::find_if(std::begin(m_Timechanging), std::end(m_Timechanging), [&channel](Channel& _channel) { return &channel == &_channel; }) != std::end(m_Timechanging);

	} while (waitingReset);

	{
		std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());
		channel.getSourceVoice()->SubmitSourceBuffer(&m_Reset_Buffer, nullptr);
		channel.getAudioBuffer().PlayBegin = static_cast<UINT32>(m_format.nSamplesPerSec * time);
		channel.getSourceVoice()->SubmitSourceBuffer(&channel.getAudioBuffer(), nullptr);
		channel.getSourceVoice()->Start();
	}
}