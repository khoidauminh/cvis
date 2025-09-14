cmake_minimum_required(VERSION 3.10.0)
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

set(SOURCES
    src/util/common.c
    src/util/audio.c
    src/util/fft.c
    src/util/logging.c
    src/util/interpolation.c

    src/render/render.c
    src/render/font.c

    src/visualizers/classic/spectrum.c
    src/visualizers/classic/vectorscope.c
    src/visualizers/classic/oscilloscope.c
    src/visualizers/classic/slice.c
    
    src/visualizers/game/snake.c
    
    src/visualizers/visualizer.c

    src/program/config.c
    src/program/program.c
    src/main.c
)

add_executable(cvis)
target_sources(cvis PRIVATE ${SOURCES})

set_property(TARGET cvis PROPERTY C_STANDARD 23)
set_property(TARGET cvis PROPERTY C_STANDARD_REQUIRED TRUE)
set_property(TARGET cvis PROPERTY C_EXTENSIONS FALSE)

target_compile_options(cvis PRIVATE
    -Wall -Wextra -Wpedantic -Wconversion
    -ffast-math -msse2 -mavx512f
    -march=native
    -fwrapv
    -O2
)

target_include_directories(cvis PRIVATE "include")
target_link_libraries(cvis PRIVATE m SDL3::SDL3 miniaudio)

install(TARGETS cvis)
