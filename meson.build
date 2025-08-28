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
  ],
)

cc = meson.get_compiler('c')
math = cc.find_library('m', required: false)
sdl3 = cc.find_library('SDL3', required: false)
ncurses = cc.find_library('ncurses', required: false)

miniaudio = dependency(
  'miniaudio',
  default_options: [
    'c_args=-DMA_ENABLE_ONLY_SPECIFIC_BACKENDS -DMA_ENABLE_PULSEAUDIO -DMA_NO_DECODING -DMA_NO_ENCODING -DMA_NO_NODE_GRAPH -DMA_NO_GENERATION',
  ],
)

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
  # c_args: ['-ffast-math'],
  install: true,
  dependencies: [sdl3, math, miniaudio, ncurses],
  include_directories: ['src/include', 'src/visualizers/'],
)
