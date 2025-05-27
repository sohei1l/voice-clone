cmake_minimum_required(VERSION 3.16)
project(echotwin VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(PORTAUDIO REQUIRED portaudio-2.0)
pkg_check_modules(SNDFILE REQUIRED sndfile)

find_package(Eigen3 REQUIRED)

add_executable(echotwin 
    src/main.cpp
    src/audio_recorder.cpp
    src/feature_extractor.cpp
)

target_include_directories(echotwin PRIVATE 
    ${PORTAUDIO_INCLUDE_DIRS}
    ${SNDFILE_INCLUDE_DIRS}
    ${EIGEN3_INCLUDE_DIR}
)

target_link_libraries(echotwin 
    ${PORTAUDIO_LIBRARIES}
    ${SNDFILE_LIBRARIES}
)

target_compile_options(echotwin PRIVATE 
    ${PORTAUDIO_CFLAGS_OTHER}
    ${SNDFILE_CFLAGS_OTHER}
)