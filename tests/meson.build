check_mode = executable('check_mode', 'check_mode.c', 'greatest.h',
    include_directories: inc,
    link_with: libdng,
    install: false,
)
check_mode = executable('check_dng', 'check_dng.c', 'greatest.h',
    include_directories: inc,
    link_with: libdng,
    install: false,
)

test('check_mode', check_mode)
test('check_dng', check_dng)