add_library(Lib_AudioPlayer STATIC)

# add source files
target_sources(Lib_AudioPlayer PRIVATE
    "Audio_System.cpp"
    "Audio_System.h"
    "AudioHelper.cpp"
    "AudioHelper.h"
    "Channel.cpp"
    "Channel.h"
    "FileProcessing.cpp"
    "FileProcessing.h"
    "FileProcessing_MP3.cpp"
    "FileProcessing_MP3.h"
    "FileProcessing_OGG.cpp"
    "FileProcessing_OGG.h"
    "FileProcessing_WAV.cpp"
    "FileProcessing_WAV.h"
    "FileProcessingFactory.cpp"
    "FileProcessingFactory.h"
    "ReverbParameters.h"
    "Sound.cpp"
    "Sound.h"
)

# link to external libraries
# mp3
target_include_directories(Lib_AudioPlayer PRIVATE "${EXTERNALS_DIRECTORY}/minimp3")
# ogg vorbis
target_link_libraries(Lib_AudioPlayer PRIVATE ogg)
target_link_libraries(Lib_AudioPlayer PRIVATE vorbis)
target_link_libraries(Lib_AudioPlayer PRIVATE vorbisfile)