hwaccess_sources = files(
  'src/hwAccessIntf.C',
  'src/hwBaseAccess.C',
  'src/hwDirectAccess.C',
  'src/hwSbeAccess.C',
  'src/hwTransport.C',
)

hwaccess_inc = include_directories(
  'include', 
)

hwaccess_lib = shared_library(
  'hwaccess',
  hwaccess_sources,
  include_directories: [libfdt_inc, targeting_inc, hwaccess_inc],
  link_with: targeting_lib,
  install: true
)

# Export public headers into /usr/include/hwaccess
install_headers('include/hwAccessIntf.H', subdir: 'hwaccess')

