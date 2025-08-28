if not meson.is_cross_build()
    gtest_dep = dependency('gtest', required: true)
    gtest_main_dep = dependency('gtest_main', required: true)

    test_sources = files('test_target_service.C')

    targeting_tests_exe = executable(
        'targeting-tests',
        test_sources,
        dependencies: [
            libfdt_dep,
            gtest_dep,
            gtest_main_dep,
            targeting_dep,
            targetsvc_dep,
        ],
    )

test('targeting-tests', targeting_tests_exe)
endif

executable(
    'transport-app',
    'transport_app.C',
    dependencies: [libfdt_dep, targeting_dep, targetsvc_dep, transport_dep],
    install: true,
)
