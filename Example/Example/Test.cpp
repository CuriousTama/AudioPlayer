#include "Audio/Sound.h"
#include <iostream>


/*
last checkup 19/09/2021	23h50

	wav :		good
	ogg :		good
	mp3 :		good
	wav Mono :	good
	ogg Mono :	good
	mp3 Mono :	good
*/


void test_Play(std::string path) {
	Sound sound(path);
	sound.play();

	std::this_thread::sleep_for(std::chrono::seconds(15));
}
void test_PauseResume(std::string path) {
	Sound sound(path);
	sound.play();

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	sound.pause();
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	sound.resume();
	std::this_thread::sleep_for(std::chrono::milliseconds(15));

	std::this_thread::sleep_for(std::chrono::seconds(5));
}
void test_MultiPlay(std::string path) {
	Sound sound(path);
	sound.play();
	std::this_thread::sleep_for(std::chrono::milliseconds(6000));
	sound.play();

	std::this_thread::sleep_for(std::chrono::milliseconds(10000));
}
void test_Stop(std::string path) {
	Sound sound(path);
	sound.play();

	std::this_thread::sleep_for(std::chrono::milliseconds(4000));
	sound.stop();

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
}
void test_Volume(std::string path) {
	Sound sound(path);
	sound.setVolume(10.f);
	sound.play();
	std::this_thread::sleep_for(std::chrono::milliseconds(4000));
	sound.setVolume(50.f);
	std::this_thread::sleep_for(std::chrono::milliseconds(4000));
	sound.setVolume(25.f);

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}
void test_Speed(std::string path) {
	Sound sound(path);
	sound.play();
	std::this_thread::sleep_for(std::chrono::milliseconds(4000));
	sound.setSpeed(125.f);

	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}
void test_Duration(std::string path) {
	Sound sound(path);
	std::cout << "Total play time : " << sound.getAudioDuration() << std::endl;
}
void test_CurrentPlaytime(std::string path) {
	Sound sound(path);
	sound.play();
	std::cout << "wait ~ 3.5 sec " << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(3500));
	std::cout << "Current play time : " << sound.getAudioTime() << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	sound.stop();
}
void test_SetAudioTime(std::string path) {
	Sound sound(path);
	sound.play();

	std::cout << "set time to middle" << std::endl;
	sound.setAudioTime(std::chrono::seconds(static_cast<int>(sound.getAudioDuration() / 2.0)));
	std::cout << "Time : " << sound.getAudioTime() << " / " << sound.getAudioDuration() << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(2000));

}
void test_Looping(std::string path) {
	Sound sound(path);
	sound.play();
	sound.setLooping(true);

	std::this_thread::sleep_for(std::chrono::seconds(int(sound.getAudioDuration()) + 10));
}
void test_3D(std::string path) {
	Sound sound(path);
	sound.enable3D(true);
	sound.setPosition(-100.f, 15.f, 0.f);
	sound.play();

	while (sound.getPosition().x < 100) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		sound.movePosition(1.f, 0.f, 0.f);
	}
}
void test_SubMixer(std::string path) {
	Audio_System::addSubMixer("Custom1");
	Sound sound(path);
	sound.setMixer("CUSTOM1");
	sound.play();

	while (Audio_System::getSubMixerVolume("custom1") > 1.f) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		Audio_System::setSubMixerVolume("custom1", Audio_System::getSubMixerVolume("custom1") - 1.f);
	}
}
void test_SubMixer3D(std::string path) {
	Audio_System::setSubMixerVolume("Custom1", 100.f);
	Sound sound(path);
	sound.setPosition(-25.f, 15.f, 0.f);
	sound.setMixer("CUSTOM1");
	sound.play();

	while (Audio_System::getSubMixerVolume("custom1") > 1.f) {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		Audio_System::setSubMixerVolume("custom1", Audio_System::getSubMixerVolume("custom1") - 1.f);
	}
}
void test_Reverb(std::string path) {
	Sound sound(path);
	sound.setReverb(Sound::Reverb::Concerthall);
	sound.play();

	std::this_thread::sleep_for(std::chrono::seconds(15));
}


void tests(std::string SoundPath)
{
	Audio_System::setListenerOrientation({ 0, 1, 0 }, { 0, 0, -1 });
	Audio_System::setMasterVolume(100.f);


	std::cout << "Test : play\n";
	test_Play(SoundPath);


	std::cout << "Test : Pause and resume\n";
	test_PauseResume(SoundPath);


	std::cout << "Test : Multi play\n";
	test_MultiPlay(SoundPath);


	std::cout << "Test : Stop\n";
	test_Stop(SoundPath);


	std::cout << "Test : Volume\n";
	test_Volume(SoundPath);


	std::cout << "Test : Speed\n";
	test_Speed(SoundPath);


	std::cout << "Test : Duration\n";
	test_Duration(SoundPath);


	std::cout << "Test : Current play time\n";
	test_CurrentPlaytime(SoundPath);


	std::cout << "Test : Set audio time\n";
	test_SetAudioTime(SoundPath);


	std::cout << "Test : Looping\n";
	test_Looping(SoundPath);


	std::cout << "Test : 3D\n";
	test_3D(SoundPath);


	std::cout << "Test : Submixer\n";
	test_SubMixer(SoundPath);


	std::cout << "Test : Submixer3D\n";
	test_SubMixer3D(SoundPath);

	std::cout << "Test : Reverb\n";
	test_Reverb(SoundPath);
}