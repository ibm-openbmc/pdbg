transport_sources = files(
  'fd_handle.C',
  'transport.C',
)

transport_inc = include_directories('..')

libtransport = shared_library(
  'transport',
  transport_sources,
  include_directories: [transport_inc],
  dependencies: [targeting_dep, libfdt_dep],
  version: '1.0.0',
  soversion: '1',  
  install: true
)

# Declare dependency for consumers
transport_dep = declare_dependency(
  link_with: libtransport,
  include_directories: transport_inc
)

# Install headers under transport
install_headers(
  'transport.H',
  subdir: 'transport',
  preserve_path: true,
)

pkgconfig = import('pkgconfig')

pkgconfig.generate(
  libraries: libtransport,
  version: meson.project_version(),
  filebase: 'libtransport',
  name: 'libtransport',
  description: 'transport library',
  subdirs: 'transport',
  requires: ['libtargeting', 'libfdt-custom']
)
