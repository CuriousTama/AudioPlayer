# Declare some options
option(BUILD_EXAMPLES "Build examples." OFF)

set(AUDIOPLAYER_SOURCE_ROOT_DIR  "${CMAKE_CURRENT_SOURCE_DIR}")

# Build projects
add_subdirectory("audioPlayer")

if (BUILD_EXAMPLES)
	add_subdirectory("example")
endif()