cmake_minimum_required(VERSION 4.0.0)
project(cvis
    VERSION 0.2.0
    LANGUAGES C
    DESCRIPTION "Personal Music Visualizer - C-based sibling of Coffeevis"
)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CURSES_NEED_NCURSES TRUE)

include(FetchContent)
FetchContent_Declare(
  miniaudio
  GIT_REPOSITORY git@github.com:mackron/miniaudio.git
  GIT_TAG "0.11.22"
  GIT_SHALLOW TRUE
  GIT_PROGRESS ON
  SYSTEM
)

FetchContent_MakeAvailable(miniaudio)

find_package(SDL3)
find_package(Curses)

set(SOURCES
    src/util/common.c
    src/util/audio.c
    src/util/fft.c
    src/util/logging.c
    src/util/interpolation.c
    src/render/render.c
    src/render/terminal.c
    src/render/sdl.c

    src/visualizers/classic/spectrum.c
    src/visualizers/classic/vectorscope.c
    src/visualizers/classic/oscilloscope.c
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
)

target_include_directories(cvis PRIVATE "include")
target_link_libraries(cvis m SDL3::SDL3 ncurses miniaudio)

option(raylib "Use Raylib" OFF)

if(raylib)
    find_package(raylib REQUIRED)
    if(raylib_FOUND)
        target_link_libraries(cvis raylib)
        target_compile_definitions(cvis PUBLIC USE_RAYLIB)
        target_sources(cvis PRIVATE "src/render/rl.c")
    else()
        message(FATAL_ERROR "Missing required Raylib dependency!")
    endif()
else()
    message(STATUS "Raylib disabled")
endif()


install(TARGETS cvis)
