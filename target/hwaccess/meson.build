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
  include_directories: [libfdt_inc, targeting_inc, hwaccess_inc],
  link_with: targeting_lib,
  install: true
)

# Export public headers into /usr/include/hwaccess
install_headers('hwAccessIntf.H', subdir: 'hwaccess')

