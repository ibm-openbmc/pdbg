project('pdbg', 'c', 'cpp',
  version: '3.6',
  meson_version: '>=0.58'
)

# SONAME values
soname_current = 6
soname_revision = 0
soname_age = 0

# Configuration header (config.h)
conf = configuration_data()

# Optional tools
m4 = find_program('m4', required: true)
dtc = find_program('dtc', required: true)
python3 = find_program('python3')

gdbserver_enabled = get_option('gdbserver')

ragel = disabler()
if gdbserver_enabled
  conf.set('GDBSERVER', 1)
  ragel = find_program('ragel', required: false)
endif

have_ragel = ragel.found()
conf.set('HAVE_RAGEL', have_ragel)

pkg_string = '"' + meson.project_name() + ' ' + meson.project_version() + '"'

conf.set('PACKAGE_STRING', pkg_string)

# libfdt
libfdt_dependency = dependency('fdt', required: false)
build_libfdt = not libfdt_dependency.found()

if build_libfdt
  subdir('libfdt')
else
  conf.set('HAVE_LIBFDT', 1)
endif

# Header check for <linux/fsi.h>
cc = meson.get_compiler('c')
if cc.has_header('linux/fsi.h')
  conf.set('HAVE_LINUX_FSI_H', 1)
endif

# Architecture detection
host_cpu = host_machine.cpu_family()
arch = 'def'
if host_cpu.startswith('arm')
  arch = 'arm'
  conf.set('TARGET_ARM', 1)
elif host_cpu.startswith('ppc')
  arch = 'ppc'
  conf.set('TARGET_PPC', 1)
endif

# Write config.h
configure_file(
  output: 'config.h',
  configuration: conf
)

m4_prog = find_program('m4')
dtc_prog = find_program('dtc')
fs = import('fs')

run_command(find_program('python3'), 'generate_dtb.py')

# Build subdirs
subdir('ccan')
subdir('libcronus')
subdir('libsbefifo')
subdir('libi2c')

subdir('libpdbg')

pkgconf = import('pkgconfig')

pkgconf.generate(
  name: 'libpdbg',
  filebase: 'pdbg',
  description: 'PDBG Debugging Library',
  version: meson.project_version(),
  libraries: libpdbg,
  subdirs: 'libpdbg',
  install_dir: get_option('libdir') / 'pkgconfig'
)

subdir('src')

