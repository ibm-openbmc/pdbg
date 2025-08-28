# Include libfdt
subdir('libfdt')
subdir('targeting')
subdir('targetsvc')
subdir('test')

# Build main executable linking with both libraries
executable(
    'targeting-app',
    'main.C',
    dependencies: [libfdt_dep,  targeting_dep, targetsvc_dep],
)
