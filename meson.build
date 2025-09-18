project(
  'targeting',
  'cpp',
  'c',
  version: '0.1',
  meson_version: '>=1.0.1',
  default_options: [
    'warning_level=3',
    'cpp_std=c++20',
  ])  

cpp = meson.get_compiler('cpp')

libfdt_dep = dependency('fdt', required: false)
if not libfdt_dep.found()
  libfdt_dep = dependency('fdt', fallback: ['libfdt', 'libfdt_dep'])
endif

subdir('target')  # Delegates to target/meson.build
