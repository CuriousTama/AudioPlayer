#pragma once
#include "FileProcessing.h"

class FileProcessing_WAV : public FileProcessing
{
public:
    virtual ~FileProcessing_WAV();

    bool load(const std::string& _path) override;
    bool onBufferEnd(Channel& channel) override;

    double getFileDuration() const override;
    double currentPlayTime(Channel& channel) const override;

    void setPlayTime(Channel& channel, double time) override;

private:
    bool getFileContent(const std::string& _path, std::unique_ptr<BYTE[]>& _fileContent, int& _fileSize) const;
    std::string isFileValid(const BYTE* _fileData, int _fileSize, const std::string& _path) const;
    std::string isFormatValid(const std::string& _path) const;

    bool collectFileFormat(const BYTE* _fileData, int _fileSize);
    bool collectData(const BYTE* _fileData, int _fileSize);

    std::mutex m_timeChanging_mutex;
    std::vector<std::reference_wrapper<Channel>> m_timeChanging;
};