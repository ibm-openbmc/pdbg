hwaccess_sources = files(
  'hw_access_intf.C',
  'hw_base_access.C',
  'hw_direct_access.C',
  'hw_sbe_access.C',
)

hwaccess_inc = include_directories('..')

hwaccess_lib = shared_library(
  'hwaccess',
  hwaccess_sources,
  include_directories: [hwaccess_inc],
  dependencies: [libfdt_dep, targeting_dep, transport_dep],
  version: '1.0.0',
  soversion: '1',
  install: true
)

# Export public headers into /usr/include/hwaccess
install_headers(
  'hw_access_intf.H',
  subdir: 'hwaccess',
  preserve_path: true,
)

hwaccess_dep = declare_dependency(
    link_with: hwaccess_lib,
    include_directories: hwaccess_inc
)

pkgconfig = import('pkgconfig')

pkgconfig.generate(
  libraries: hwaccess_lib,
  version: meson.project_version(),
  filebase: 'libhwaccess',
  name: 'libhwaccess',
  description: 'Hardware access library',
  subdirs: 'hwaccess',
  requires: ['libfdt-custom', 'libtargeting', 'libtransport']
)
