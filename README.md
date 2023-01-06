# Audio Player 

Audio Player is a library for Windows platform using XAudio2 2D and 3D.<br />
Supported audio files extensions are [.wav](https://en.wikipedia.org/wiki/WAV), [.ogg](https://en.wikipedia.org/wiki/Ogg), [.mp3](https://en.wikipedia.org/wiki/MP3).

C++20 is prerequisite for successfully compile. 

You can access xaudio2 stuff and more with a [Define macro](https://github.com/Tama-sama/AudioPlayer/wiki). 

## Documentation

See [wiki](https://github.com/Tama-sama/AudioPlayer/wiki).<br />
You can also find the source code of [audio player](https://github.com/Tama-sama/AudioPlayer/tree/main/Audio%20player) and a quick [example](https://github.com/Tama-sama/AudioPlayer/tree/main/Example) program made with it.

## About this project

This project is a project done between two years of school.<br />
I started this project on a whim without knowing a lot about audio programming, so I learn on the job.

### Goal :
- Make an audio player library that support some base sound formats and the user can extend it without modifying the source code.

### Problem encounters and how I have resolved them : 
- Process correctly Ogg and Mp3 files : use some dependencies.  
- 3D Sound : Documentation.

### What I've learned :
- WAV files structure.
- Export libraries.
- Reinforced multi-threading knowledge.
- Reinforced knowledge of the window library.
- Reinforced file reading knowledge.
- Clearly comment the code.
