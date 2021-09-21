#pragma once
#include "Audio_System.h"

class FileProcessing
{
public:
	/*
		virtual destructor.
	 */
	virtual ~FileProcessing() = default;



	/*
	 * Load data from file.
	 *
	 * @param path to the file.
	 * @return bool : if successfully loaded.
	 */
	virtual bool load(const std::string path) = 0;



	/*
	 * Use for streams, called on play function of channel class.
	 *
	 * @param channel used.
	 */
	virtual void play(class Channel& channel) {}



	/*
	 * Callback used by the sound card when the current playing buffer end. 
	 *
	 * @param channel conserned.
	 */
	virtual bool OnBufferEnd(class Channel& channel) = 0;



	/*
	 * Give the file total duration.
	 *
	 * @return as seconds.
	 */
	virtual double fileDuration() = 0;

	/*
	 * Get the current reading position.
	 *
	 * @param concerned channel.
	 * @return as seconds.
	 */
	virtual double currentPlayTime(class Channel& channel) = 0;

	/*
	 * Set the reading position to a current time.
	 *
	 * @param concerned channel.
	 * @param time as seconds.
	 */
	virtual void setPlayTime(class Channel& channel, double time) = 0;



	/*
	 * Get the number of byte of pData.
	 *
	 * @param concerned channel.
	 * @return number of byte of pData.
	 */
	virtual const UINT32 get_nBytes(class Channel& channel) const { return m_NumBytes; }

	/*
	 * Get current loaded samples.
	 *
	 * @param concerned channel.
	 * @return loaded samples.
	 */
	virtual const BYTE* const get_pData(class Channel& channel) const { return m_pData.get(); }



	/*
	 * Get format information.
	 *
	 * @return WAVEFORMATEX.
	 */
	const WAVEFORMATEX& getFormat() const { return m_format; }

protected:
	UINT32 m_NumBytes{ 0 };
	std::unique_ptr<BYTE[]> m_pData;
	WAVEFORMATEX m_format{ 0 };

private:

};