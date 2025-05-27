cmake_minimum_required(VERSION 3.16)
project(echotwin VERSION 1.0.0 DESCRIPTION "Lightweight C++ Voice Cloning CLI")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
pkg_check_modules(SNDFILE REQUIRED sndfile)

find_package(Eigen3 REQUIRED)

# ONNX Runtime
find_library(ONNXRUNTIME_LIB onnxruntime HINTS /usr/local/lib /opt/homebrew/lib)
if(NOT ONNXRUNTIME_LIB)
    message(FATAL_ERROR "ONNX Runtime library not found")
endif()

find_path(ONNXRUNTIME_INCLUDE onnxruntime_cxx_api.h HINTS /usr/local/include /opt/homebrew/include)
if(NOT ONNXRUNTIME_INCLUDE)
    message(FATAL_ERROR "ONNX Runtime headers not found")
endif()

add_executable(echotwin 
    src/main.cpp
    src/audio_recorder.cpp
    src/feature_extractor.cpp
    src/voice_trainer.cpp
    src/speech_synthesizer.cpp
)

target_include_directories(echotwin PRIVATE 
    ${PORTAUDIO_INCLUDE_DIRS}
    ${SNDFILE_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
    ${ONNXRUNTIME_INCLUDE}
)

target_link_libraries(echotwin 
    ${PORTAUDIO_LIBRARIES}
    ${SNDFILE_LIBRARIES}
    ${ONNXRUNTIME_LIB}
)

target_compile_options(echotwin PRIVATE 
    ${PORTAUDIO_CFLAGS_OTHER}
    ${SNDFILE_CFLAGS_OTHER}
)

# Static linking options for release builds
option(STATIC_BUILD "Build with static linking" OFF)

if(STATIC_BUILD)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    set_target_properties(echotwin PROPERTIES LINK_SEARCH_START_STATIC 1)
    set_target_properties(echotwin PROPERTIES LINK_SEARCH_END_STATIC 1)
endif()

# Cross-platform presets
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(echotwin PRIVATE -O3 -march=native)
    set_target_properties(echotwin PROPERTIES STRIP ON)
endif()