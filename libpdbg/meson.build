git_sha = run_command(
  ['git', '--work-tree=' + meson.project_source_root(), '--git-dir=' + meson.project_source_root() + '/.git', 'describe', '--always', '--long', '--dirty'],
  check: false
).stdout().strip()

libpdbg_cflags = ['-Wall', '-Werror', '-DGIT_SHA1="@0@"'.format(git_sha == '' ? 'unknown' : git_sha)]

dtb_s_files = []
dtb_list_file = meson.project_source_root() / 'generated_dtb_files.txt'

if fs.exists(dtb_list_file)
  dtb_lines = fs.read(dtb_list_file).strip().split('\n')
  foreach line : dtb_lines
    dtb_s_files += line + '.S'
  endforeach
endif

libpdbg_sources = files(
  'adu.c', 'bmcfsi.c', 'cfam.c', 'chip.c', 'cronus.c', 'debug.c',
  'device.c', 'dtb.c', 'fake.c', 'host.c', 'htm.c', 'hwunit.c',
  'i2c.c', 'i2c_kernel.c', 'kernel.c', 'libpdbg.c', 'ocmb.c', 'p8chip.c',
  'p9chip.c', 'p9_fapi_targets.c', 'p10chip.c', 'p10_fapi_targets.c',
  'sbefifo.c', 'sbe_api.c', 'sprs.c', 'target.c', 'thread.c'
) + dtb_s_files

generated_header_paths = []
header_list_path = 'generated_headers.txt'

if fs.exists(header_list_path)
  header_lines = fs.read_file(header_list_path).strip().split('\n')
  foreach path : header_lines
    # Treat each as relative path for Meson
    generated_header_paths += [files(path)]
  endforeach
endif

libpdbg_includes = include_directories('.', '../libfdt', '../ccan', '../libsbefifo', '../m4')
link_args = ['-z', 'noexecstack']

libpdbg = library('pdbg',
  sources: libpdbg_sources + generated_header_paths,
  include_directories: libpdbg_includes,
  c_args: libpdbg_cflags,
  link_with: [libcronus, libsbefifo, libi2c, libfdt],
  link_args: link_args,
  dependencies: [ccan_dep, libfdt_dep],
  version: '6.0.0',  # SONAME: 6:0:0
  install: true
)

install_headers(
  'libpdbg.h', 'libpdbg_sbe.h',
)

