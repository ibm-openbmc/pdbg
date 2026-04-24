cronus_sources = files(
  'buffer.c',
  'cfam.c',
  'connect.c',
  'request.c',
  'sbefifo.c',
  'scom.c',
  'proxy.c',  # used by cronus_proxy executable
)

cronus_headers = files(
  'buffer.h',
  'instruction.h',
  'libcronus.h',
  'libcronus_private.h',
)

libcronus = static_library('cronus',
  sources: cronus_sources,
  include_directories: include_directories('.'),
  c_args: ['-Wall', '-g'],
  install: false
)

# cronus_proxy executable (if needed)
cronus_proxy = executable('cronus_proxy',
  'proxy.c',
  include_directories: include_directories('.'),
  dependencies: [],
  link_with: [libcronus],
  c_args: ['-Wall', '-g'],
  install: false
)
