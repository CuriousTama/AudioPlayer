#pragma once
#include "FileProcessing.h"

#include <array>
#include <fstream>
#include <vorbis/vorbisfile.h>

class FileProcessing_OGG : public FileProcessing
{
private:
    struct StreamDatas
    {
        static constexpr size_t bufferSize{ 4096 };

        OggVorbis_File m_vf;
        std::array<BYTE, bufferSize> m_buffer;
        std::ifstream m_stream;
        bool m_end{ false };

        UINT32 m_numBytes{ 0 };
    };

public:
    virtual ~FileProcessing_OGG();

    bool load(const std::string& _path) override;
    void play(Channel& channel) override;
    bool onBufferEnd(Channel& channel) override;

    UINT32 get_nBytes(Channel& channel) const override;
    const BYTE* get_pData(Channel& channel) const override;
    double getFileDuration() const override;
    double currentPlayTime(Channel& channel) const override;

    void setPlayTime(Channel& channel, double time) override;

private:
    ov_callbacks getCallbacks() const;
    void removeStream(Channel& channel);

    std::map<Channel*, StreamDatas> m_currentsStreams;
    std::string m_path;

    OggVorbis_File m_info_vf;
    std::ifstream m_info_stream;
};