#pragma once

#include <xaudio2.h>
#include <xaudio2fx.h>
#include <string>
#include <mutex>

class Channel
{
public:
private:

	class VoiceCallback : public IXAudio2VoiceCallback
	{
	protected:
		void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 SamplesRequired) override {}
		void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
		void STDMETHODCALLTYPE OnStreamEnd() override {}
		void STDMETHODCALLTYPE OnBufferStart(void* pBufferContext) override {}
		void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override;
		void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext) override {}
		void STDMETHODCALLTYPE OnVoiceError(void* pBufferContext, HRESULT Error) override {}
	};

public:

	/*
	 * Constructor.
	 *
	 * @param Mixer to play.
	 * @param Sound to play.
	 */
	Channel(const std::string Mixer, class Sound& sound);

	/*
		Destructor.
	 */
	~Channel();



	/*
		Start playing the sound.
	 */
	void play();

	/*
		Resume paused sound.
	 */
	void resume();

	/*
		Pause the sound.
	 */
	void pause();

	/*
		Stop the sound.
	 */
	void Stop();



	/*
	 * Give the Mixer name.
	 *
	 * @return string : Mixer name.
	 */
	const std::string getMixerName() const;



	/*
	 * Set volume.
	 *
	 * @param volume to set in percentage (min : 0.f, Max : 100.f).
	 */
	void setVolume(float volume);



	/*
	 * Set speed.
	 *
	 * @param speed to set in percentage.
	 */
	void setSpeed(float speed);

	/*
	 * Give the speed.
	 *
	 * @return float : speed in 1.f = 100 %.
	 */
	const float getSpeed();



	/*
	 * get the linked sound. (const)
	 *
	 * @return const sound.
	 */
	const class Sound& getSound() const;



	/*
	   Enable 3D.
	 */
	void enable3D();



	/*
	 * Do calculation for 3D.
	 *
	 * @param bool : force calculate even if the 3D isn't enabled.
	 */
	void calculate3D(bool force = false);



	/*
	 * Set customs reverb parameters.
	 *
	 * @param XAUDIO2FX_REVERB_I3DL2_PARAMETERS or Sound::Reverb.
	 */
	void setReverb(const XAUDIO2FX_REVERB_I3DL2_PARAMETERS& rev);

	/*
	 * Set customs reverb parameters.
	 *
	 * @param XAUDIO2FX_REVERB_PARAMETERS.
	 */
	void setReverb(const XAUDIO2FX_REVERB_PARAMETERS& rev);

private:

	inline static VoiceCallback vcb;
	XAUDIO2_BUFFER m_Audio_Buffer;
	IXAudio2SourceVoice* m_pSource = nullptr;
	std::string m_mixer;
	bool m_forceStop{ false };
	class Sound& m_rSound;
	std::mutex m_pSource_mutex;

	std::unique_ptr<XAUDIO2_SEND_DESCRIPTOR[]> m_sendDescriptors;
	XAUDIO2_VOICE_SENDS m_sendList;

	IUnknown* m_pReverbEffect = nullptr;
	XAUDIO2_EFFECT_DESCRIPTOR m_effects;
	XAUDIO2_EFFECT_CHAIN m_effectChain;
	IXAudio2SubmixVoice* m_reverb = nullptr;
	XAUDIO2FX_REVERB_PARAMETERS m_native;

#ifdef _AUDIO_ADMIN_
public:

	/*
	 * Get Source voice pointer.
	 *
	 * @return IXAudio2SourceVoice pointer.
	 */
	IXAudio2SourceVoice* getSourceVoice() {
		return m_pSource;
	}

	/*
	 * Get mutex for the Source voice.
	 *
	 * @return std::mutex.
	 */
	std::mutex& getSourceVoiceMutex() {
		return m_pSource_mutex;
	}



	/*
	 * Get the forceStop variable.
	 *
	 * @return if the force stop is true.
	 */
	bool isForcedStop() {
		return m_forceStop;
	}



	/*
	 * Get audioBuffer.
	 *
	 * @return XAUDIO2_BUFFER.
	 */
	XAUDIO2_BUFFER& getAudioBuffer() {
		return m_Audio_Buffer;
	}

#endif // _AUDIO_ADMIN_

};
