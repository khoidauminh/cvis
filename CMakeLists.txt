cmake_minimum_required(VERSION 3.24.0)
project(cvis
    VERSION 0.3.0
    LANGUAGES C
    DESCRIPTION "Personal Music Visualizer - C-based sibling of Coffeevis"
)

set(CMAKE_BUILD_TYPE None)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(OpenGL_GL_PREFERENCE GLVND)
set(FETCHCONTENT_QUIET OFF)

include(FetchContent)

find_package(miniaudio)

if (NOT miniaudio_FOUND)
    FetchContent_Declare(
        miniaudio
        GIT_REPOSITORY https://github.com/mackron/miniaudio
        GIT_TAG "0.11.22"
        GIT_SHALLOW TRUE
        GIT_PROGRESS ON
        SYSTEM
    )

    FetchContent_MakeAvailable(miniaudio)
endif()

find_package(SDL3)

if (NOT SDL3_FOUND)
 FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG "release-3.2.22"
        GIT_SHALLOW TRUE
        GIT_PROGRESS ON
        SYSTEM
    )

    FetchContent_MakeAvailable(SDL3)
endif()

set(SOURCES_CVIS
    src/util/common.c
    src/util/audio.c
    src/util/fft.c
    src/util/logging.c
    src/util/interpolation.c

    src/render/render.c
    src/render/font.c

    src/visualizers/visualizer.c

    src/program/config.c
    src/program/program.c
)

set(SOURCES_CVIS_VISUALIZERS
    src/visualizers/classic/spectrum.c
    src/visualizers/classic/vectorscope.c
    src/visualizers/classic/oscilloscope.c
    src/visualizers/classic/slice.c
    
    src/visualizers/game/snake/snake.c
)

add_library(cvisinternals STATIC ${SOURCES_CVIS})
target_include_directories(cvisinternals PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(cvisinternals PRIVATE miniaudio SDL3::SDL3)

add_library(cvisvisualizers STATIC ${SOURCES_CVIS_VISUALIZERS})
target_include_directories(cvisvisualizers PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/public)
target_link_libraries(cvisvisualizers PRIVATE cvisinternals)

add_executable(cvis src/main.c)

set_property(TARGET cvis cvisinternals cvisvisualizers PROPERTY C_STANDARD 23)
set_property(TARGET cvis cvisinternals cvisvisualizers PROPERTY C_STANDARD_REQUIRED TRUE)
set_property(TARGET cvis cvisinternals cvisvisualizers PROPERTY C_EXTENSIONS FALSE)

set(COMPILE_FLAGS
    -Wall -Wextra -Wpedantic -Wconversion
    -ffast-math -msse2 -mavx512f
    -march=native
    -fwrapv
    -O2
)

target_compile_options(cvisinternals PRIVATE ${COMPILE_FLAGS})
target_compile_options(cvisvisualizers PRIVATE ${COMPILE_FLAGS})

target_include_directories(cvis PRIVATE "include")
target_link_libraries(cvis PRIVATE cvisinternals cvisvisualizers m)

install(TARGETS cvis)
