#pragma once
#include "Audio_System.h"

class Channel;

class FileProcessing
{
public:
    virtual ~FileProcessing() = default;

    // Load data from file.
    virtual bool load(const std::string& _path) = 0;

    // Use for streams, called on play function of channel class.
    virtual void play(Channel& channel) {}

    // Callback used by the sound card when the current playing buffer end.
    virtual bool onBufferEnd(Channel& channel) = 0;

    // Give the file total duration.
    virtual double getFileDuration() const = 0;

    // Get the current reading position.
    virtual double currentPlayTime(Channel& channel) const = 0;

    // Set the reading position to a current time.
    virtual void setPlayTime(Channel& channel, double time) = 0;

    // Get the number of byte of pData.
    virtual UINT32 get_nBytes(Channel& channel) const { return m_numBytes; }

    // Get current loaded samples.
    virtual const BYTE* get_pData(Channel& channel) const { return m_pData.get(); }

    // Get format information.
    const WAVEFORMATEX& getFormat() const { return m_format; }

protected:
    UINT32 m_numBytes{ 0 };
    std::unique_ptr<BYTE[]> m_pData;
    WAVEFORMATEX m_format{ 0 };
};