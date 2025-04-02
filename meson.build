project('junowm','c')

wayland_server = dependency('wayland-server')
wayland_client = dependency('wayland-client')
executable('junowm','src/server.c', dependencies: [wayland_server])
executable('testclient','src/client.c', dependencies: [wayland_client])
