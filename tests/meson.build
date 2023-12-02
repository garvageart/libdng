check_mode = executable('check_mode', 'check_mode.c', 'greatest.h',
    include_directories: inc,
    link_with: libdng,
    install: false,
)
check_dng = executable('check_dng', 'check_dng.c', 'greatest.h',
    include_directories: inc,
    link_with: libdng,
    dependencies: libtiff,
    install: false,
)
check_dcp = executable('check_dcp', 'check_dcp.c', 'greatest.h',
    include_directories: inc,
    link_with: libdng,
    dependencies: libtiff,
    install: false,
)

subdir('fixture')

test('check_mode', check_mode)
test('check_dng', check_dng)
test('check_dcp', check_dcp, workdir: meson.current_build_dir())