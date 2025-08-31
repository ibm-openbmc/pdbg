# Include libfdt
subdir('libfdt')
subdir('targeting')
subdir('transport')
subdir('hwaccess')
subdir('targetsvc')
subdir('test')

# Build main executable linking with both libraries
executable(
    'targeting-app',
    'main.C',
    dependencies: [libfdt_dep,  targeting_dep, targetsvc_dep],
)


executable(
    'transport-app',
    'transport_app.C',
    dependencies: [libfdt_dep, targeting_dep, targetsvc_dep, transport_dep],
    install: true,
)
