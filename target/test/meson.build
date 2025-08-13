gtest_dep = dependency('gtest', required: true)
gtest_main_dep = dependency('gtest_main', required: true)

test_sources = files(
  'test_target_service.C',
)

test_inc = include_directories(
  '..',
  '../targeting',
  '../targeting/common',
  '../targeting/predicates',
  '../targeting/xmltohb',
)

targeting_tests_exe = executable('targeting-tests',
  test_sources,
  include_directories: [libfdt_inc, targeting_inc, test_inc],
  dependencies: [libfdt_dep, gtest_dep, gtest_main_dep],
  link_with: targeting_lib,
)

test('targeting-tests', targeting_tests_exe)

