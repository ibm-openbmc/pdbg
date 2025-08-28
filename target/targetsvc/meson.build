service_lib_sources = files(
    'dtree_loader.C',
    'target_devtree_map.C',
    'target_service.C',
)

targetsvc_inc = include_directories('.')

targetsvc_lib = shared_library(
    'targetsvc',
    service_lib_sources,
    include_directories: [libfdt_inc, targetsvc_inc, targeting_inc],
    dependencies: libfdt_dep,
    link_with: targeting_lib,
    install: true,
)

# Export public headers into /usr/include/target
install_headers('target_service.H', subdir: 'targeting')

