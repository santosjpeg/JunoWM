project('junowm','c')

# Dependencies
wayland_protocols = dependency('wayland-protocols')
wlroots = dependency('wlroots-0.19')
wayland_server = dependency('wayland-server')

# Project args 
add_project_arguments('-DWLR_USE_UNSTABLE', language: 'c')

# Wayland protocol headers
protocols_server_header = wayland_protocols.get_pkgconfig_variable('pkgdatadir')

wayland_protocols_proto = custom_target('xdg-shell-protocol',
  input: protocols_server_header / 'stable/xdg-shell/xdg-shell.xml',
  output: ['xdg-shell-protocol.h', 'xdg-shell-protocol.c'],
  command: [
    find_program('wayland-scanner'), 'server-header', '@INPUT@', '@OUTPUT0@',
    '&&',
    find_program('wayland-scanner'), 'private-code', '@INPUT@', '@OUTPUT1@'
  ]
)


# Sources
executable('junowm', 
  sources: ['src/server.c', wayland_protocols_proto],
  include_directories: include_directories('.'),
  dependencies: [wayland_protocols, wlroots, wayland_server]
)
