#include "FileProcessing_OGG.h"
#include "Channel.h"
#include "Sound.h"

#include <assert.h>
#include <iostream>

namespace OGGFileProcessing
{
    // ----------------------------------------------
    size_t read(void* buffer, size_t elementSize, size_t elementCount, void* dataSource)
    {
        assert(elementSize == 1);

        std::ifstream* stream = static_cast<std::ifstream*>(dataSource);
        stream->read(static_cast<char*>(buffer), elementCount);
        const size_t bytesRead = static_cast<size_t>(stream->gcount());
        stream->clear(); // In case we read past EOF
        return bytesRead;
    }

    // ----------------------------------------------
    int seek(void* dataSource, ogg_int64_t offset, int origin)
    {
        static const std::vector<std::ios_base::seekdir> seekDirections{ std::ios_base::beg, std::ios_base::cur, std::ios_base::end };

        std::ifstream& stream = *static_cast<std::ifstream*>(dataSource);
        stream.seekg(offset, seekDirections.at(origin));
        stream.clear(); // In case we seeked to EOF
        return 0;
    }

    // ----------------------------------------------
    long tell(void* dataSource)
    {
        std::ifstream& stream = *static_cast<std::ifstream*>(dataSource);
        const auto position = stream.tellg();
        assert(position >= 0);
        return static_cast<long>(position);
    }
} // namespace OGGFileProcessing

// ----------------------------------------------
FileProcessing_OGG::~FileProcessing_OGG()
{
    for (auto& current : m_currentsStreams)
    {
        ov_clear(&current.second.m_vf);
    }

    m_currentsStreams.clear();
    ov_clear(&m_info_vf);
}

// ----------------------------------------------
bool FileProcessing_OGG::load(const std::string& _path)
{
    if (m_info_stream.is_open())
    {
        m_info_stream.close();
    }

    m_path = _path;

    m_info_stream.open(m_path, std::ios::binary);

    if (ov_open_callbacks(&m_info_stream, &m_info_vf, NULL, 0, getCallbacks()) < 0)
    {
        std::string error("[error] : File does not appear to be an Ogg bitstream : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
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

// ----------------------------------------------
void FileProcessing_OGG::play(Channel& channel)
{
    StreamDatas& Current = m_currentsStreams[&channel];
    Current.m_stream.open(m_path, std::ios::binary);

    if (ov_open_callbacks(&Current.m_stream, &Current.m_vf, NULL, 0, getCallbacks()) < 0)
    {
        removeStream(channel);
        std::string error("[error] : Fail to open callbacks OGG\n[File] " __FILE__ "\n[Line] " + std::to_string(__LINE__));
        throw std::exception(error.c_str());
    }

    long ret = ov_read(&Current.m_vf, std::bit_cast<char*, BYTE*>(Current.m_buffer.data()), Current.bufferSize, 0, 2, 1, nullptr);
    Current.m_numBytes = ret;
}

// ----------------------------------------------
bool FileProcessing_OGG::onBufferEnd(Channel& channel)
{
    if (!m_currentsStreams.contains(&channel))
        return true;

    StreamDatas& Current = m_currentsStreams.find(&channel)->second;

    if (Current.m_end || channel.isForcedStop())
    {
        removeStream(channel);
        return true;
    }

    int current_section = 0;
    long ret = ov_read(&Current.m_vf, std::bit_cast<char*, BYTE*>(Current.m_buffer.data()), Current.bufferSize, 0, 2, 1, &current_section);

    if (ret > 0) // next buffer part
    {
        Current.m_numBytes = ret;
        channel.getAudioBuffer().AudioBytes = Current.m_numBytes;

        std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());
        channel.getSourceVoice()->SubmitSourceBuffer(&channel.getAudioBuffer(), nullptr);
        return false;
    }

    if (channel.getSound().isLooping())
    {
        ov_time_seek(&Current.m_vf, 0.0);
        this->onBufferEnd(channel);
        return false;
    }

    Current.m_end = true;
    removeStream(channel);
    return true;
}

// ----------------------------------------------
UINT32 FileProcessing_OGG::get_nBytes(Channel& channel) const
{
    if (m_currentsStreams.contains(&channel))
        return m_currentsStreams.find(&channel)->second.m_numBytes;

    return m_numBytes;
}

// ----------------------------------------------
const BYTE* FileProcessing_OGG::get_pData(Channel& channel) const
{
    if (m_currentsStreams.contains(&channel))
        return m_currentsStreams.find(&channel)->second.m_buffer.data();

    return m_pData.get();
}

// ----------------------------------------------
double FileProcessing_OGG::getFileDuration() const
{
    OggVorbis_File& info_vf = const_cast<OggVorbis_File&>(m_info_vf);
    return ov_time_total(&info_vf, -1);
}

// ----------------------------------------------
double FileProcessing_OGG::currentPlayTime(Channel& channel) const
{
    if (!m_currentsStreams.contains(&channel))
        return 0.0;

    StreamDatas& Current = const_cast<StreamDatas&>(m_currentsStreams.find(&channel)->second);
    return ov_time_tell(&Current.m_vf);
}

// ----------------------------------------------
void FileProcessing_OGG::setPlayTime(Channel& channel, double time)
{
    if (!m_currentsStreams.contains(&channel))
        return;

    StreamDatas& Current = m_currentsStreams.find(&channel)->second;
    ov_time_seek(&Current.m_vf, time);
}

// ----------------------------------------------
ov_callbacks FileProcessing_OGG::getCallbacks() const
{
    return ov_callbacks{ OGGFileProcessing::read, OGGFileProcessing::seek, nullptr, OGGFileProcessing::tell };
}

// ----------------------------------------------
void FileProcessing_OGG::removeStream(Channel& channel)
{
    if (!m_currentsStreams.contains(&channel))
        return;

    ov_clear(&m_currentsStreams[&channel].m_vf);
    m_currentsStreams.erase(&channel);
}