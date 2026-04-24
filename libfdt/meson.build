libfdt_sources = files(
  'fdt_addresses.c',
  'fdt.c',
  'fdt_empty_tree.c',
  'fdt_overlay.c',
  'fdt_ro.c',
  'fdt_rw.c',
  'fdt_strerror.c',
  'fdt_sw.c',
  'fdt_wip.c',
)

libfdt_headers = files(
  'libfdt.h',
  'libfdt_env.h',
  'fdt.h'
)

libfdt = static_library('fdt', libfdt_sources,
  include_directories: include_directories('.'),
  install: true
)

libfdt_dep = declare_dependency(
  link_with: libfdt,
  include_directories: include_directories('.')
)

install_headers(libfdt_headers)
