libi2c_sources = files(
  'libi2c.c',
  'smbus.c'
)

libi2c_inc = include_directories('.')

libi2c = static_library(
  'i2c',
  libi2c_sources,
  include_directories: libi2c_inc,
  c_args: ['-Wall', '-Werror'],
  install: false
)

install_headers('libi2c.h', 'smbus.h')
