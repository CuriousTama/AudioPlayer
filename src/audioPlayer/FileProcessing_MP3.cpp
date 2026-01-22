#if (!MINIMP3_ONLY_SIMD) && _MSC_VER
    /* x64 always have SSE2, arm64 always have neon, no need for generic code */
    #define MINIMP3_ONLY_SIMD // error in x86 platform if not defined
#endif // (!MINIMP3_ONLY_SIMD) && _MSC_VER

#define MINIMP3_IMPLEMENTATION
#include "FileProcessing_MP3.h"
#include "Channel.h"
#include "Sound.h"

// ----------------------------------------------
FileProcessing_MP3::~FileProcessing_MP3()
{
    for (auto& current : m_currentsStreams)
    {
        mp3dec_ex_close(&current.second.mp3d);
    }
}

// ----------------------------------------------
bool FileProcessing_MP3::load(const std::string& _path)
{
    m_path = _path;
    mp3dec_ex_t mp3d;
    if (mp3dec_ex_open(&mp3d, m_path.c_str(), MP3D_SEEK_TO_SAMPLE) < 0)
    {
        std::string error("[error] : File does not appear to be an Mp3 file : " + _path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
        throw std::exception(error.c_str());
    }

    m_format.wFormatTag = WAVE_FORMAT_PCM;
    m_format.nChannels = mp3d.info.channels;
    m_format.nSamplesPerSec = mp3d.info.hz;
    m_format.wBitsPerSample = 16;
    m_format.nBlockAlign = m_format.nChannels * (m_format.wBitsPerSample / 8);
    m_format.nAvgBytesPerSec = m_format.nChannels * m_format.nSamplesPerSec * (m_format.wBitsPerSample / 8);
    m_format.cbSize = 0;

    if (m_format.nSamplesPerSec == 0)
    {
        mp3dec_ex_close(&mp3d);
        return false;
    }

    m_audioDuration = (mp3d.samples * 2 / mp3d.info.channels) * sizeof(mp3d_sample_t) / static_cast<double>(m_format.nSamplesPerSec) / 4.0;
    mp3dec_ex_close(&mp3d);

    return true;
}

// ----------------------------------------------
void FileProcessing_MP3::play(Channel& channel)
{
    StreamDatas& Current = m_currentsStreams[&channel];

    if (mp3dec_ex_open(&Current.mp3d, m_path.c_str(), MP3D_SEEK_TO_SAMPLE) < 0)
    {
        removeStream(channel);
        std::string error("[error] : File does not appear to be an Mp3 file : " + m_path + "\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
        throw std::exception(error.c_str());
    }

    size_t readed = mp3dec_ex_read(&Current.mp3d, (mp3d_sample_t*)Current.samplesBuffer.data(), Current.samplesToRead);

    if (Current.mp3d.last_error != 0)
    {
        removeStream(channel);
        std::string error("[error] : An error occurred while reading the file : [" + m_path + "] with error code : [" + std::to_string(Current.mp3d.last_error) + "]\n[File] " + __FILE__ + "\n[Line] " + std::to_string(__LINE__));
        throw std::exception(error.c_str());
    }

    Current.numBytes = static_cast<UINT32>(readed * sizeof(mp3d_sample_t));
}

// ----------------------------------------------
bool FileProcessing_MP3::onBufferEnd(Channel& channel)
{
    if (!m_currentsStreams.contains(&channel))
    {
        removeStream(channel);
        return true;
    }

    StreamDatas& Current = m_currentsStreams.find(&channel)->second;

    if (!Current.isEnded && !channel.isForcedStop())
    {
        size_t readed = mp3dec_ex_read(&Current.mp3d, (mp3d_sample_t*)Current.samplesBuffer.data(), Current.samplesToRead);

        if (readed > 0)
        {
            Current.numBytes = static_cast<UINT32>(readed * sizeof(mp3d_sample_t));
            channel.getAudioBuffer().AudioBytes = Current.numBytes;

            std::lock_guard<std::mutex> lock(channel.getSourceVoiceMutex());
            channel.getSourceVoice()->SubmitSourceBuffer(&channel.getAudioBuffer(), nullptr);
        }
        else if (channel.getSound().isLooping())
        {
            mp3dec_ex_seek(&Current.mp3d, 0);
            this->onBufferEnd(channel);
        }
        else
        {
            mp3dec_ex_seek(&Current.mp3d, 0);
            Current.isEnded = true;
            removeStream(channel);
            return true;
        }
    }
    else
    {
        mp3dec_ex_seek(&Current.mp3d, 0);
        removeStream(channel);
        return true;
    }

    return false;
}

// ----------------------------------------------
UINT32 FileProcessing_MP3::get_nBytes(Channel& channel) const
{
    if (m_currentsStreams.contains(&channel))
        return m_currentsStreams.find(&channel)->second.numBytes;

    return m_numBytes;
}

// ----------------------------------------------
const BYTE* FileProcessing_MP3::get_pData(Channel& channel) const
{
    if (m_currentsStreams.contains(&channel))
        return m_currentsStreams.find(&channel)->second.samplesBuffer.data();

    return m_pData.get();
}

// ----------------------------------------------
double FileProcessing_MP3::getFileDuration() const
{
    return m_audioDuration;
}

// ----------------------------------------------
double FileProcessing_MP3::currentPlayTime(Channel& channel) const
{
    if (!m_currentsStreams.contains(&channel))
        return 0.0;

    const StreamDatas& Current = m_currentsStreams.find(&channel)->second;

    if (m_format.nSamplesPerSec != 0 && Current.mp3d.info.channels != 0)
    {
        return Current.mp3d.cur_sample * (2 / Current.mp3d.info.channels) / sizeof(mp3d_sample_t) / static_cast<double>(m_format.nSamplesPerSec);
    }

    return 0.0;
}

// ----------------------------------------------
void FileProcessing_MP3::setPlayTime(Channel& channel, double time)
{
    if (!m_currentsStreams.contains(&channel))
        return;

    StreamDatas& Current = m_currentsStreams.find(&channel)->second;

    if (m_format.nSamplesPerSec != 0 && Current.mp3d.info.channels != 0)
    {
        mp3dec_ex_seek(&Current.mp3d, static_cast<uint64_t>((m_format.nSamplesPerSec * 2.0) * time / (2 / Current.mp3d.info.channels)));
    }
}

// ----------------------------------------------
void FileProcessing_MP3::removeStream(Channel& channel)
{
    m_currentsStreams.erase(&channel);
}