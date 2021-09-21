#pragma once
#include "FileProcessing.h"

#include <vorbis/vorbisfile.h>
#include <fstream>

class FileProcessing_OGG : public FileProcessing
{
public:

	/*
		virtual destructor.
	 */
	virtual ~FileProcessing_OGG();



	/*
	 * Get the number of byte of pData.
	 *
	 * @param concerned channel.
	 * @return number of byte of pData.
	 */
	virtual const UINT32 get_nBytes(class Channel& channel) const override;

	/*
	 * Get current loaded samples.
	 *
	 * @param concerned channel.
	 * @return loaded samples.
	 */
	virtual const BYTE* const get_pData(class Channel& channel) const override;



	/*
	 * Load data from file.
	 *
	 * @param path to the file.
	 * @return bool : if successfully loaded.
	 */
	virtual bool load(const std::string path) override;



	/*
	 * Use for streams, called on play function of channel class.
	 *
	 * @param channel used.
	 */
	virtual void play(class Channel& channel) override;



	/*
	 * Callback used by the sound card when the current playing buffer end.
	 *
	 * @param channel conserned.
	 */
	virtual bool OnBufferEnd(class Channel& channel) override;



	/*
	 * erase a channel from currents open streams.
	 *
	 * @param channel conserned.
	 */
	void removeStream(class Channel& channel);



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
	static const size_t BufferSize{ 4096 };

	struct S_Ogg_stream
	{
		OggVorbis_File m_vf;
		std::array<BYTE, BufferSize> m_buffer;
		std::ifstream m_stream;
		bool m_end{ false };

		UINT32 m_NumBytes{ 0 };
		std::unique_ptr<BYTE[]> m_pData;
	};

	std::map<class Channel*, S_Ogg_stream> m_Currents_streams;
	std::string m_path;

	OggVorbis_File m_info_vf;
	std::ifstream m_info_stream;
};