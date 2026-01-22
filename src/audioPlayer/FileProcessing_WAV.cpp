#include "FileProcessing_WAV.h"
#include "Channel.h"
#include "Sound.h"
#include <fstream>
#include <iostream>

// ----------------------------------------------
FileProcessing_WAV::~FileProcessing_WAV()
{
    std::lock_guard<std::mutex> lock(m_timeChanging_mutex);
}

// ----------------------------------------------
bool FileProcessing_WAV::load(const std::string& _path)
{
    int fileSize = 0;
    std::unique_ptr<BYTE[]> fileContent;
    if (!getFileContent(_path, fileContent, fileSize))
    {
        std::string error("[error] : Failed to open : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
        throw std::exception(error.c_str());
    }

    std::string validationResult = isFileValid(fileContent.get(), fileSize, _path);
    if (validationResult.empty())
    {
        throw std::exception(validationResult.c_str());
    }

    if (!collectFileFormat(fileContent.get(), fileSize))
    {
        std::string error("[error] : fmt chunk not found : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
        throw std::exception(error.c_str());
    }

    std::string formatValidationResult = isFormatValid(_path);
    if (formatValidationResult.empty())
    {
        throw std::exception(formatValidationResult.c_str());
    }

    if (!collectData(fileContent.get(), fileSize))
    {
        std::string error("[error] : data chunk not found : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
        throw std::exception(error.c_str());
    }

    return true;
}

bool FileProcessing_WAV::getFileContent(const std::string& _path, std::unique_ptr<BYTE[]>& _fileContent, int& _fileSize) const
{
    std::ifstream file(_path, std::ios::binary);

    if (!file)
        return false;

    file.seekg(0, std::ios::end);
    _fileSize = static_cast<int>(file.tellg());
    file.seekg(0, std::ios::beg);

    _fileContent = std::make_unique<BYTE[]>(_fileSize);
    file.read(std::bit_cast<char*>(_fileContent.get()), _fileSize);

    return true;
}

// ----------------------------------------------
std::string FileProcessing_WAV::isFileValid(const BYTE* _fileData, int _fileSize, const std::string& _path) const
{
    if (*std::bit_cast<const int*>(&_fileData[0]) != 'FFIR')
        return std::string("[error] : bad fourCC : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));

    if (_fileSize <= 44) // 44 is start of data chunk
        return std::string("[error] : file too small : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));

    if (*std::bit_cast<const int*>(&_fileData[8]) != 'EVAW')
        return std::string("[error] : format not WAVE : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));

    return "";
}

// ----------------------------------------------
bool FileProcessing_WAV::collectFileFormat(const BYTE* _fileData, int _fileSize)
{
    bool bFilledFormat = false;
    for (size_t i = 12; i < static_cast<size_t>(_fileSize);)
    {
        if (*std::bit_cast<const int*>(&_fileData[i]) == ' tmf')
        {
            memcpy(&m_format, &_fileData[i + 8], sizeof(m_format));
            bFilledFormat = true;
            break;
        }
        // chunk size + size entry size + chunk id entry size + word padding
        i += (*std::bit_cast<const int*>(&_fileData[i + 4]) + 9) & 0xFFFFFFFE;
    }

    m_format.cbSize = 0;

    return bFilledFormat;
}

// ----------------------------------------------
std::string FileProcessing_WAV::isFormatValid(const std::string& _path) const
{
    if (m_format.wFormatTag != WAVE_FORMAT_PCM)
        return std::string("[error] : bad wave format (wFormatTag) : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));

    if (m_format.nChannels != 1 && m_format.nChannels != 2)
        return std::string("[error] : bad wave format (nChannels) : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));

    if (m_format.wBitsPerSample != 8 && m_format.wBitsPerSample != 16 && m_format.wBitsPerSample != 24 && m_format.wBitsPerSample != 32)
        return std::string("[error] : bad wave format (wBitsPerSample) : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));

    if (m_format.nSamplesPerSec <= 0)
        return std::string("[error] : bad wave format (nSamplesPerSec) : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));

    if (m_format.nBlockAlign != m_format.nChannels * (m_format.wBitsPerSample / 8))
        return std::string("[error] : bad wave format (nBlockAlign) : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));

    if (m_format.nAvgBytesPerSec != m_format.nChannels * m_format.nSamplesPerSec * (m_format.wBitsPerSample / 8))
        return std::string("[error] : bad wave format (nAvgBytesPerSec) : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));

    return "";
}

// ----------------------------------------------
bool FileProcessing_WAV::collectData(const BYTE* _fileData, int _fileSize)
{
    for (size_t i = 12; i < static_cast<size_t>(_fileSize);)
    {
        const int chunkSize = *std::bit_cast<const int*>(&_fileData[i + 4]);
        if (*std::bit_cast<const int*>(&_fileData[i]) == 'atad')
        {
            m_pData = std::make_unique<BYTE[]>(chunkSize);
            m_numBytes = chunkSize;
            memcpy(m_pData.get(), &_fileData[i + 8], m_numBytes);

            return true;
        }
        // chunk size + size entry size + chunk id entry size + word padding
        i += (chunkSize + 9) & 0xFFFFFFFE;
    }

    return false;
}

// ----------------------------------------------
bool FileProcessing_WAV::onBufferEnd(Channel& channel)
{
    std::lock_guard<std::mutex> lock(m_timeChanging_mutex);
    auto it = std::find_if(std::begin(m_timeChanging), std::end(m_timeChanging), [&channel](Channel& _channel) {
        return &channel == &_channel;
    });

    bool TimeChange = it != std::end(m_timeChanging);

    if (channel.getSound().isLooping() && !channel.isForcedStop())
    {
        std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());

        channel.getAudioBuffer().PlayBegin = 0;
        channel.getSourceVoice()->SubmitSourceBuffer(&channel.getAudioBuffer(), nullptr);
    }
    else if (TimeChange)
    {
        m_timeChanging.erase(it);
    }
    else
    {
        return true;
    }

    return false;
}

// ----------------------------------------------
double FileProcessing_WAV::getFileDuration() const
{
    if (m_format.nSamplesPerSec == 0)
        return 0.0;

    return (m_numBytes * 2 / m_format.nChannels) / static_cast<double>(m_format.nSamplesPerSec) / 4.0;
}

// ----------------------------------------------
double FileProcessing_WAV::currentPlayTime(Channel& channel) const
{
    if (m_format.nSamplesPerSec == 0)
        return 0.0;

    std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());

    XAUDIO2_VOICE_STATE tmp;
    channel.getSourceVoice()->GetState(&tmp);

    return (channel.getAudioBuffer().PlayBegin + tmp.SamplesPlayed) / static_cast<double>(m_format.nSamplesPerSec);
}

// ----------------------------------------------
void FileProcessing_WAV::setPlayTime(Channel& channel, double time)
{
    {
        std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());
        std::lock_guard<std::mutex> lock2(m_timeChanging_mutex);

        m_timeChanging.push_back(channel);
        channel.getSourceVoice()->Stop();
        channel.getSourceVoice()->FlushSourceBuffers();
    }

    bool waitingReset = false;
    do
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        std::lock_guard<std::mutex> lock(m_timeChanging_mutex);
        waitingReset = std::find_if(std::begin(m_timeChanging), std::end(m_timeChanging), [&channel](Channel& _channel) { return &channel == &_channel; }) != std::end(m_timeChanging);
    }
    while (waitingReset);

    {
        std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());
        channel.getAudioBuffer().PlayBegin = static_cast<UINT32>(m_format.nSamplesPerSec * time);
        channel.getSourceVoice()->SubmitSourceBuffer(&channel.getAudioBuffer(), nullptr);
        channel.getSourceVoice()->Start();
    }
}