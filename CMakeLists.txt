cmake_minimum_required(VERSION 3.31)
project(AudioPlayer)
set(CMAKE_CXX_STANDARD 20)

# Declare some options
option(BUILD_EXAMPLES "Build examples." OFF)

# Add and build externals
add_subdirectory("external/ogg")
add_subdirectory("external/vorbis")

# Declare variables
set(EXTERNALS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/external")

# Build projects
add_subdirectory("src")