project('marina','c','cpp',license : 'BSD-3',
    default_options : ['c_std=c11', 'cpp_std=c++14']
)

egl            = dependency('egl')
elesv2         = dependency('glesv2')
wlr            = dependency('wlroots')
wayland_server = dependency('wayland-server')
wayland_proto  = dependency('wayland-protocols')
wayland_client = dependency('wayland-client')
xkbcommon      = dependency('xkbcommon')
pixman         = dependency('pixman-1')

incdir         = include_directories('src')

subdir('protos')

executable(
    'marina-compositor',
    [
        'src/main.cpp',
        'src/server.cpp',
        'src/input.cpp',
        'src/cursor.cpp',
        'src/seat.cpp',
        'src/output.cpp',
        'src/view.cpp',
        'src/wlview.cpp',
        'src/xdgview.cpp',
        'src/xdgview_v6.cpp',
        'src/renderer.cpp'
    ],
    dependencies : [
        egl,
        elesv2,
        wlr,
        wayland_server,
        wayland_proto,
        wlr_protos,
        xkbcommon,
        pixman
    ],
    include_directories : [
        incdir
    ]
)
