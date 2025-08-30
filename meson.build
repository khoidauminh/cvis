project(
  'cvis',
  'c',
  license: 'GPLv3',
  license_files: ['LICENSE'],
  version: '0.1.0',
  meson_version: '>=1.7',
  default_options: [
    'c_std=c23',
    'warning_level=3',
    'buildtype=release',
    'c_args=-ffast-math',
  ],
)

cc = meson.get_compiler('c')

depencendies = [
  cc.find_library('m', required: false),
  dependency('SDL3'),
  dependency('ncurses'),
  dependency(
    'miniaudio',
    default_options: [
      'decoding=false',
      'encoding=false',
      'resource_manager=disabled',
      'generation=false',

      'dr_wav=false',
      'dr_flac=false',
      'dr_mp3=false',
      'examples=disabled',
      'tests=disabled',
    ],
  ),
  dependency('raylib', required: get_option('raylib')),
]

sources = [
  'src/util/common.c',
  'src/util/audio.c',
  'src/util/fft.c',
  'src/util/logging.c',
  'src/util/interpolation.c',
  'src/render/render.c',
  'src/render/terminal.c',
  'src/render/sdl.c',
  'src/visualizers/classic/spectrum.c',
  'src/visualizers/classic/vectorscope.c',
  'src/visualizers/visualizer.c',
  'src/program/config.c',
  'src/program/program.c',
  'src/main.c',
]

if get_option('raylib')
  add_project_arguments('-DUSE_RAYLIB', language: 'c')
  sources += 'src/render/rl.c'
endif

executable(
  'cvis',
  sources,
  install: true,
  dependencies: depencendies,
  include_directories: ['include'],
)
