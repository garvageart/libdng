check_mode = executable('check_mode', 'check_mode.c', 'greatest.h',
    include_directories: inc,
    link_with: libdng,
    install: false,
)
test('check_mode', check_mode)