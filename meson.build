project('libdng', 'c',
    version: '0.1.0',
    license: 'GPL',
)

libtiff = dependency('libtiff-4')

# We use libtool-version numbers because it's easier to understand.
# Before making a release, the libdng_so_*
# numbers should be modified. The components are of the form C:R:A.
# a) If binary compatibility has been broken (eg removed or changed interfaces)
#    change to C+1:0:0.
# b) If interfaces have been changed or added, but binary compatibility has
#    been preserved, change to C+1:0:A+1
# c) If the interface is the same as the previous version, change to C:R+1:A
libdng_lt_c=1
libdng_lt_r=0
libdng_lt_a=0

libdng_so_version = '@0@.@1@.@2@'.format((libdng_lt_c - libdng_lt_a),
                                              libdng_lt_a,
                                              libdng_lt_r)

inc = include_directories('include')
install_headers('include/libdng.h')

lib_src = [
    'src/libdng.c',
    'src/dng.h',
    'src/mode.c',
    'src/mode.h',
]

add_project_arguments(['-Wno-multichar'], language: 'c')

libdng = shared_library('dng', lib_src,
    version: libdng_so_version,
    include_directories: inc,
    dependencies: libtiff,
    install: true
)

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries: libdng,
                version: libdng_so_version,
                name: 'libdng',
                filebase: 'libdng',
                description: 'The wrapper library to generate DNG files with libtiff')

executable('makedng', 'util/makedng.c',
    link_with: libdng,
    include_directories: inc,
    install: true,
)
