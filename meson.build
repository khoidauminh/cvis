project(
  'cvis',
  'c',
  version: '0.1.0',
  default_options: [
    'c_std=c23',
    'warning_level=3',
    'buildtype=release',
    'c_args=-ffast-math',
    'auto-features=disabled',
    'default_both_libraries=static',
    'prefer_static=true',
  ],
)

cc = meson.get_compiler('c')
math = cc.find_library('m', required: false)
sdl3 = dependency('SDL3')
ncurses = dependency('ncurses')
miniaudio = dependency('miniaudio')

sources = [
  'util/common.c',
  'util/audio.c',
  'util/fft.c',
  'util/logging.c',
  'util/interpolation.c',
  'render/render.c',
  'render/terminal.c',
  'render/window.c',
  'visualizers/classic/spectrum.c',
  'visualizers/classic/vectorscope.c',
  'visualizers/visualizer.c',
  'program/config.c',
  'program/program.c',
  'main.c',
]

executable(
  'cvis',
  sources,
  install: true,
  dependencies: [sdl3, math, miniaudio, ncurses],
  include_directories: ['include', 'visualizers/'],
)
