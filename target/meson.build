# Include libfdt
subdir('libfdt')
subdir('targeting')
subdir('hwaccess')
subdir('targetsvc')
subdir('test')

# Build main executable linking with both libraries
executable(
    'targeting-app',
    'main.C',
    include_directories: [libfdt_inc, targeting_inc, service_inc],
    dependencies: libfdt_dep,
    link_with: [targeting_lib, targetsvc_lib],
)
