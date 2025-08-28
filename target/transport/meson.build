transport_sources = files(
  'fd_handle.C',
  'transport.C',
)

transport_inc = include_directories('.')

libtransport = shared_library(
  'transport',
  transport_sources,
  include_directories: [transport_inc],
  dependencies: [targeting_dep, libfdt_dep],
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
  subdir: 'transport'
)

