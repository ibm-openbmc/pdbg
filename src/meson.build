src_includes = [
  include_directories('.'),
  include_directories('..')
]

# Base sources
pdbg_sources = files(
  'cfam.c', 'htm.c', 'istep.c', 'i2c.c', 'main.c', 'mem.c', 'optcmd.c',
  'parsers.c', 'path.c', 'pdbgproxy.c', 'progress.c', 'reg.c', 'ring.c',
  'scom.c', 'thread.c', 'util.c'
)

# Optional gdb_parser
if get_option('gdbserver') and have_ragel
  gdb_parser = custom_target('gdb_parser.c',
    input: 'gdb_parser.rl',
    output: 'gdb_parser.c',
    command: [ragel_prog, '-o', '@OUTPUT@', '@INPUT@']
  )
  pdbg_sources += gdb_parser
else
  pdbg_sources += files('gdb_parser_precompile.c')
endif

executable('pdbg',
  pdbg_sources,
  include_directories: src_includes + [libpdbg_includes],
  c_args: ['-Wall', '-Werror', '-DGIT_SHA1="@0@"'.format(git_sha == '' ? 'unknown' : git_sha)],
  link_with: libpdbg,
  dependencies: [libfdt_dep, ccan_dep],
  install: true
)

# Tests
test_sources = {
  'hexdump_test': ['util.c', 'tests/hexdump_test.c'],
  'optcmd_test': ['optcmd.c', 'parsers.c', 'tests/optcmd_test.c'],
  'libpdbg_target_test': ['tests/libpdbg_target_test.c'],
  'libpdbg_release_dt_root_test': ['tests/libpdbg_release_dt_root_test.c'],
  'libpdbg_probe_test1': ['tests/libpdbg_probe_test.c'],
  'libpdbg_probe_test2': ['tests/libpdbg_probe_test.c'],
  'libpdbg_probe_test3': ['tests/libpdbg_probe_test.c'],
  'libpdbg_dtree_test': ['tests/libpdbg_dtree_test.c'],
  'libpdbg_prop_test': ['tests/libpdbg_prop_test.c'],
  'libpdbg_attr_test': ['tests/libpdbg_attr_test.c'],
  'libpdbg_traverse_test': ['tests/libpdbg_traverse_test.c']
}

foreach name, srcs : test_sources
  c_args = []
  if name.startswith('libpdbg_probe_test')
    test_id = name[-1]
    c_args += ['-DTEST_ID=' + test_id]
  endif

  executable(name,
    files(srcs),
    include_directories: [src_includes, libpdbg_includes],
    link_with: libpdbg,
    c_args: c_args + ['-Wall', '-g'],
    install: false
  )
endforeach

# C++ tests
executable('libpdbg_p9_fapi_translation_test',
  files('tests/libpdbg_p9_fapi_translation_test.C', 'tests/p9_scominfo.C'),
  cpp_args: ['-Wall', '-g'],
  link_with: libpdbg,
  include_directories: [src_includes, libpdbg_includes],
  install: false
)

executable('libpdbg_p10_fapi_translation_test',
  files('tests/libpdbg_p10_fapi_translation_test.C', 'tests/p10_scominfo.C', 'tests/p10_scom_addr.C'),
  cpp_args: ['-Wall', '-g'],
  link_with: libpdbg,
  include_directories: [src_includes, libpdbg_includes],
  install: false
)
