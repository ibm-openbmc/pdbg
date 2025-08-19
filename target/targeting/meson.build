targeting_lib_sources = files(
    'common/entitypath.C',
    'predicates/predicatepostfixexpr.C',
    'target.C',
    'xmltohb/targAttrIdToName.C',
)

targeting_inc = include_directories('..', '.', 'xmltohb', 'common', 'predicates')

targeting_lib = shared_library(
    'targeting',
    targeting_lib_sources,
    include_directories: [libfdt_inc, targeting_inc],
    dependencies: libfdt_dep,
    install: true,
)

# Export public headers into /usr/include/target
install_headers(
    'target.H',
    'association.H',
    'xmltohb/attributeenums.H',
    'xmltohb/attributetraits.H',
    'xmltohb/attributestructs.H',
    'common/entitypath.H',
    'predicates/predicateattr.H',
    'predicates/predicateattrval.H',
    'predicates/predicatebase.H',
    'predicates/predicateisfunctional.H',
    'predicates/predicatepostfixexpr.H',
    subdir: 'targeting',
)

