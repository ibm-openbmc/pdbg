libpdbg_test_deps = [libpdbg, libi2c, libcronus, libsbefifo]
libpdbg_test_inc = include_directories('../libpdbg', '../libfdt')

c_args_common = ['-Wall', '-Werror']
link_args_common = ['-Wl,--whole-archive', '-lpdbg', '--no-whole-archive']

# C Tests
executable(
  'libpdbg_target_test',
  'src/tests/libpdbg_target_test.c',
  include_directories: libpdbg_test_inc,
  dependencies: libpdbg_test_deps,
  c_args: c_args_common,
  link_args: link_args_common,
  install: false
)
test('target_test', find_program('libpdbg_target_test'))

executable(
  'libpdbg_release_dt_root_test',
  'src/tests/libpdbg_release_dt_root_test.c',
  include_directories: libpdbg_test_inc,
  dependencies: libpdbg_test_deps,
  c_args: c_args_common,
  link_args: link_args_common,
  install: false
)
test('release_dt_root_test', find_program('libpdbg_release_dt_root_test'))

# Probe tests 1-3
foreach i : [1, 2, 3]
  test_name = 'libpdbg_probe_test' + i.to_string()
  executable(
    test_name,
    'src/tests/libpdbg_probe_test.c',
    include_directories: libpdbg_test_inc,
    dependencies: libpdbg_test_deps,
    c_args: c_args_common + ['-DTEST_ID=' + i.to_string()],
    link_args: link_args_common,
    install: false
  )
  test(test_name, find_program(test_name))
endforeach

# Other C test executables
tests_src = [
  ['libpdbg_dtree_test', 'src/tests/libpdbg_dtree_test.c'],
  ['libpdbg_prop_test', 'src/tests/libpdbg_prop_test.c'],
  ['libpdbg_attr_test', 'src/tests/libpdbg_attr_test.c'],
  ['libpdbg_traverse_test', 'src/tests/libpdbg_traverse_test.c'],
  ['optcmd_test', ['src/optcmd.c', 'src/parsers.c', 'src/tests/optcmd_test.c']]
]

foreach t : tests_src
  exe = executable(
    t.get(0),
    t.get(1),
    include_directories: libpdbg_test_inc,
    dependencies: libpdbg_test_deps,
    c_args: c_args_common,
    link_args: link_args_common,
    install: false
  )
  test(t.get(0), exe)
endforeach

# C++ Tests
executable(
  'libpdbg_p9_fapi_translation_test',
  ['src/tests/libpdbg_p9_fapi_translation_test.C', 'src/tests/p9_scominfo.C'],
  include_directories: libpdbg_test_inc,
  dependencies: libpdbg_test_deps,
  cpp_args: c_args_common,
  link_args: link_args_common,
  install: false
)
test('p9_fapi_translation_test', find_program('libpdbg_p9_fapi_translation_test'))

executable(
  'libpdbg_p10_fapi_translation_test',
  ['src/tests/libpdbg_p10_fapi_translation_test.C', 'src/tests/p10_scominfo.C', 'src/tests/p10_scom_addr.C'],
  include_directories: libpdbg_test_inc,
  dependencies: libpdbg_test_deps,
  cpp_args: c_args_common,
  link_args: link_args_common,
  install: false
)
test('p10_fapi_translation_test', find_program('libpdbg_p10_fapi_translation_test'))

# Shell script tests
shell_tests = [
  'tests/test_selection.sh',
  'tests/test_selection2.sh',
  'tests/test_hw_bmc.sh',
  'tests/test_hexdump.sh',
  'tests/test_tree.sh',
  'tests/test_tree2.sh',
  'tests/test_prop.sh',
  'tests/test_attr_array.sh',
  'tests/test_attr_packed.sh',
  'tests/test_traverse.sh',
  'tests/test_p9_fapi_translation.sh',
  'tests/test_p10_fapi_translation.sh',
]

foreach script : shell_tests
  test(script, find_program(script))
endforeach
