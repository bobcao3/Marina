project('marina','c','cpp',license : 'BSD-3')

egl            = dependency('egl')
elesv2         = dependency('glesv2')
wlr            = dependency('wlroots')
wayland_server = dependency('wayland-server')
wayland_proto  = dependency('wayland-protocols')
wayland_client = dependency('wayland-client')

incdir         = include_directories('src')

subdir('protos')

executable(
    'marina-compositor',
    [
        'src/main.cpp',
        'src/server.cpp',
        'src/output.cpp',
        'src/xdgview.cpp',
        'src/renderer.cpp'
    ],
    dependencies : [
        egl,
        elesv2,
        wlr,
        wayland_server,
        wayland_proto,
        wlr_protos
    ],
    include_directories : [
        incdir
    ]
)
