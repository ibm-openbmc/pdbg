service_lib_sources = files(
    'dtree_loader.C',
    'target_devtree_map.C',
    'target_service.C',
)

targetsvc_inc = include_directories('..')

targetsvc_lib = shared_library(
  'targetsvc',
  service_lib_sources,
  include_directories: targetsvc_inc,
  dependencies: [libfdt_dep, targeting_dep, hwaccess_dep],
  version: '1.0.0',
  soversion: '1',
  install: true,
)

# Export public headers into /usr/include/target
install_headers(
  'target_service.H',
  'target_devtree_map.H',
  'dtree_loader.H',
  subdir: 'targeting',
  preserve_path: true,
)

targetsvc_dep = declare_dependency(
  link_with: targetsvc_lib,
  include_directories: targetsvc_inc,
  dependencies: [targeting_dep, libfdt_dep, hwaccess_dep],
)

pkgconfig = import('pkgconfig')

pkgconfig.generate(
  libraries: targetsvc_lib,
  version: meson.project_version(),
  filebase: 'libtargetsvc',
  name: 'libtargetsvc',
  description: 'Targeting service library',
  subdirs: 'targeting',
  requires: ['libtargeting', 'libfdt-custom', 'libhwaccess']
)
