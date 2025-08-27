project(
  'coffeevis',
  'c',
  version: '0.0.0',
  default_options: [
    'debug=false',
    'strip=true',
    'c_std=c23',
    'warning_level=3',
    'buildtype=release',
  ],
)

cc = meson.get_compiler('c')
math = cc.find_library('m', required: true)
sdl3 = cc.find_library('SDL3', required: true)
ncurses = cc.find_library('ncurses', required: false)
miniaudio = dependency('miniaudio')

sources = [
  'src/util/common.c',
  'src/util/audio.c',
  'src/util/fft.c',
  'src/util/logging.c',
  'src/util/interpolation.c',

  'src/render/render.c',
  'src/render/terminal.c',
  'src/render/window.c',

  'src/visualizers/classic/spectrum.c',
  'src/visualizers/classic/vectorscope.c',

  'src/program/program.c',

  'src/main.c',
]

executable(
  'cvis',
  sources,
  dependencies: [sdl3, math, miniaudio, ncurses],
  include_directories: include_directories(['src/include', 'src/visualizers/']),
)
