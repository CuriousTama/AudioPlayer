add_executable(Lib_AudioPlayer_Use_Examples)

# add source files
target_sources(Lib_AudioPlayer_Use_Examples PRIVATE
    "Example.cpp"
    "main.cpp"
    "test.cpp"
)

target_include_directories(Lib_AudioPlayer_Use_Examples PUBLIC ${AUDIOPLAYER_SOURCE_ROOT_DIR})

# link audio player lib
target_link_libraries(Lib_AudioPlayer_Use_Examples PRIVATE Lib_AudioPlayer)