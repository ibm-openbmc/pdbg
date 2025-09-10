targeting_lib_sources = files(
    'common/entitypath.C',
    'predicates/predicatepostfixexpr.C',
    'target.C',
    'xmltohb/targAttrIdToName.C',
)

targeting_inc = include_directories('..', 'xmltohb')

targeting_lib = shared_library(
    'targeting',
    targeting_lib_sources,
    include_directories: [targeting_inc],
    dependencies: libfdt_dep,
    version: '1.0.0',
    soversion: '1',    
    install: true,
)

# Export public headers into /usr/include/targeting
install_headers(
    'target.H',
    'association.H',
    'target_tmpl.H',
    'xmltohb/attributeenums.H',
    'xmltohb/attributetraits.H',
    'xmltohb/attributestructs.H',
    'xmltohb/builtins.h',
    'xmltohb/targAttrIdToName.H',
    'common/entitypath.H',
    'predicates/predicateattr.H',
    'predicates/predicateattrval.H',
    'predicates/predicatebase.H',
    'predicates/predicateisfunctional.H',
    'predicates/predicatepostfixexpr.H',
    subdir: 'targeting',
    preserve_path: true,
)

# Declare dependency for consumers
targeting_dep = declare_dependency(
    link_with: targeting_lib,
    include_directories: targeting_inc
)

pkgconfig = import('pkgconfig')

pkgconfig.generate(
  libraries: targeting_lib,
  version: meson.project_version(),
  filebase: 'libtargeting',
  name: 'libtargeting',
  description: 'Targeting library',
  subdirs: 'targeting',
  requires: ['libfdt']
)
