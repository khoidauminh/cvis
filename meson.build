project(
  'cvis',
  'c',
  version: '0.1.0',
  meson_version: '>=1.1',
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
raylib = dependency('raylib', required: get_option('raylib'))
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
  'render/sdl.c',
  'visualizers/classic/spectrum.c',
  'visualizers/classic/vectorscope.c',
  'visualizers/visualizer.c',
  'program/config.c',
  'program/program.c',
  'main.c',
]

if get_option('raylib')
  add_project_arguments('-DUSE_RAYLIB', language: 'c')
  sources += 'render/rl.c'
endif

executable(
  'cvis',
  sources,
  install: true,
  dependencies: [sdl3, raylib, math, miniaudio, ncurses],
  include_directories: ['include', 'visualizers/'],
)
