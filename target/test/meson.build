if not meson.is_cross_build()
  gtest_dep = dependency('gtest', required: true)
  gtest_main_dep = dependency('gtest_main', required: true)

  # Test sources
  test_sources = files(
    'test_target_service.C',
  )

  # Include directories for test-specific headers
  test_inc = include_directories('..')

  # Build test executable
  targeting_tests_exe = executable(
    'targeting-tests',
    test_sources,
    include_directories: [test_inc],
    dependencies: [gtest_dep, gtest_main_dep, libfdt_dep, targeting_dep, targetsvc_dep],
  )

  # Register the test
  test('targeting-tests', targeting_tests_exe)
endif  
