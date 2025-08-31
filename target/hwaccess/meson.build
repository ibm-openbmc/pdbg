hwaccess_sources = files(
  'hwAccessIntf.C',
  'hwBaseAccess.C',
  'hwDirectAccess.C',
  'hwSbeAccess.C',
)

hwaccess_inc = include_directories(
  '.',
)

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
install_headers('hwAccessIntf.H', subdir: 'hwaccess')

hwaccess_dep = declare_dependency(
    link_with: hwaccess_lib,
    include_directories: hwaccess_inc,
    dependencies: targeting_dep
)

