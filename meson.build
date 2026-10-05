project('evdevd', 'c',
        meson_version: '>= 1.3.0',
        version: '0.1.0',
        default_options: [ 'warning_level=3', 'c_std=c23' ])

add_project_arguments([
  '-D_GNU_SOURCE',
  '-Wno-pedantic',
  '-Wno-unused-parameter',
], language: 'c')

sources = [
  'src/evdevd.c',
  'src/rule.c',
  'src/xmalloc.c',
]

executable('evdevd', sources,
           install: true)

