# Include libfdt
subdir('libfdt')
subdir('targeting')
subdir('targetsvc')
subdir('transport')
subdir('test')

# Main executable
executable(
    'targeting-app',
    'main.C',
    include_directories: [libfdt_inc, targeting_inc, targetsvc_inc, transport_inc],
    dependencies: libfdt_dep,
    link_with: [targeting_lib, targetsvc_lib, libtransport],
    install: true,
)

# transportexecutable
executable(
    'transport-app',
    'transport_app.C',
    include_directories: [libfdt_inc, targeting_inc, targetsvc_inc, transport_inc],
    dependencies: libfdt_dep,
    link_with: [targeting_lib, targetsvc_lib, libtransport],
    install: true,
)

