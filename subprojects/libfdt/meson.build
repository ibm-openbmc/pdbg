project('libfdt', 'c', version: '1.7.2')

libfdt_sources = files(
  'fdt.c',
  'fdt_ro.c',
  'fdt_rw.c',
  'fdt_strerror.c',
  'fdt_sw.c',
  'fdt_wip.c',
  'fdt_empty_tree.c',
  'fdt_addresses.c',
  'fdt_overlay.c',
)

libfdt_inc = include_directories('.')

libfdt_lib = static_library(
  'fdt',
  libfdt_sources,
  include_directories: libfdt_inc,
  install: false 
)

libfdt_dep = declare_dependency(
  include_directories: libfdt_inc,
  link_with: libfdt_lib
)

meson.override_dependency('fdt', libfdt_dep)
