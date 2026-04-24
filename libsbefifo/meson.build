libsbefifo_sources = files(
  'cmd_array.c',
  'cmd_control.c',
  'cmd_dump.c',
  'cmd_generic.c',
  'cmd_instruction.c',
  'cmd_memory.c',
  'cmd_mpipl.c',
  'cmd_register.c',
  'cmd_ring.c',
  'cmd_scom.c',
  'connect.c',
  'ffdc.c',
  'operation.c'
)

libsbefifo_inc = include_directories('.')

libsbefifo = static_library(
  'sbefifo',
  libsbefifo_sources,
  include_directories: libsbefifo_inc,
  c_args: ['-Wall', '-Werror'],
  install: false
)

install_headers('libsbefifo.h')
