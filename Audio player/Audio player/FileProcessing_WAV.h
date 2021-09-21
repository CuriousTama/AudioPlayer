#pragma once
#include "FileProcessing.h"

class FileProcessing_WAV : public FileProcessing
{
public:
	/*
		default constructor.
	 */
	FileProcessing_WAV();

	/*
		virtual destructor.
	 */
	virtual ~FileProcessing_WAV();



	/*
	 * Load data from file.
	 *
	 * @param path to the file.
	 * @return bool : if successfully loaded.
	 */
	virtual bool load(const std::string path) override;



	/*
	 * Callback used by the sound card when the current playing buffer end.
	 *
	 * @param channel conserned.
	 */
	virtual bool OnBufferEnd(class Channel& channel) override;



	/*
	 * Give the file total duration.
	 *
	 * @return as seconds.
	 */
	virtual double fileDuration() override;

	/*
	 * Get the current reading position.
	 *
	 * @param concerned channel.
	 * @return as seconds.
	 */
	virtual double currentPlayTime(class Channel& channel) override;

	/*
	 * Set the reading position to a current time.
	 *
	 * @param concerned channel.
	 * @param time as seconds.
	 */
	virtual void setPlayTime(class Channel& channel, double time) override;

protected:

private:
	inline static XAUDIO2_BUFFER m_Reset_Buffer;
	inline static std::unique_ptr<BYTE[]> m_Reset_data;
	std::mutex m_TimeChanging_mutex;
	std::vector<std::reference_wrapper<class Channel>> m_Timechanging;
};