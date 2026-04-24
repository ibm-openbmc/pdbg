ccan_headers = files(
  'array_size/array_size.h',
  'build_assert/build_assert.h',
  'check_type/check_type.h',
  'container_of/container_of.h',
  'cppmagic/cppmagic.h',
  'list/list.h',
  'short_types/short_types.h',
  'str/str.h'
)

ccan_sources = files(
  'list/list.c',
  'str/str.c'
)

include_dirs = include_directories('.', '..')

ccan_lib = static_library('ccan', ccan_sources,
  include_directories: include_dirs,
  install: false  # not installing it system-wide
)

ccan_dep = declare_dependency(
  include_directories: include_dirs,
  link_with: ccan_lib,
)