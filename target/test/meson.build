gtest_dep = dependency('gtest', main: true, disabler: true, required: false)
gmock_dep = dependency('gmock', disabler: true, required: false)

if not gtest_dep.found() or not gmock_dep.found()
    gtest_proj = import('cmake').subproject('googletest', required: false)
    if gtest_proj.found()
        gtest_dep = declare_dependency(
            dependencies: [
                dependency('threads'),
                gtest_proj.dependency('gtest'),
                gtest_proj.dependency('gtest_main'),
            ],
        )
        gmock_dep = gtest_proj.dependency('gmock')
    else
        assert(
            not get_option('tests').enabled(),
            'Googletest is required if tests are enabled',
        )
    endif
endif

test_sources = files('test_target_service.C')

targeting_tests_exe = executable(
    'targeting-tests',
    test_sources,
    dependencies: [
        libfdt_dep,
        targeting_dep,
        targetsvc_dep,
        gtest_dep,
        gmock_dep,
    ],
)

test('targeting-tests', targeting_tests_exe)

executable(
    'transport-app',
    'transport_app.C',
    dependencies: [libfdt_dep, targeting_dep, targetsvc_dep, transport_dep],
)

# Build main executable linking with both libraries
executable(
    'targeting-app',
    'targeting_app.C',
    dependencies: [libfdt_dep,  targeting_dep, targetsvc_dep],
)

executable(
    'performance-app',
    'performance.C',
    dependencies: [libfdt_dep,  targeting_dep, targetsvc_dep],
)
