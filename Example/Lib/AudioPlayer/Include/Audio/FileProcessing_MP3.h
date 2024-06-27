#pragma once
#include "FileProcessing.h"
#pragma warning(push)

#pragma warning(disable : 4267)
#pragma warning(disable : 4244)
#pragma warning(disable : 6385)
#pragma warning(disable : 6386)
#pragma warning(disable : 26451)
#pragma warning(disable : 6262)
#pragma warning(disable : 6011)

#include "minimp3/minimp3_ex.h"

#pragma warning(pop)

#include <array>

class FileProcessing_MP3 : public FileProcessing
{
private:
    struct StreamDatas
    {
        static constexpr int samplesToRead{ MINIMP3_MAX_SAMPLES_PER_FRAME };

        mp3dec_ex_t mp3d;
        bool isEnded{ false };

        UINT32 numBytes{ 0 };
        std::array<BYTE, samplesToRead * sizeof(mp3d_sample_t)> samplesBuffer;
    };

public:
    virtual ~FileProcessing_MP3();

    bool load(const std::string& _path) override;
    void play(Channel& channel) override;
    bool onBufferEnd(Channel& channel) override;

    UINT32 get_nBytes(Channel& channel) const override;
    const BYTE* get_pData(Channel& channel) const override;
    double getFileDuration() const override;
    double currentPlayTime(Channel& channel) const override;

    void setPlayTime(Channel& channel, double time) override;

private:
    void removeStream(Channel& channel);

    std::string m_path;
    std::map<Channel*, StreamDatas> m_currentsStreams;
    double m_audioDuration{ 0.0 };
};
