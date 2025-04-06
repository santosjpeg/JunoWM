project('junowm','c')

# Dependencies
wayland_server = dependency('wayland-server')
wayland_client = dependency('wayland-client')
wayland_protocols = dependency('wayland-protocols')
wlroots = dependency('wlroots-0.19')


# Project args 
add_project_arguments('-DWLR_USE_UNSTABLE', language: 'c')

# Wayland protocol headers
protocols_server_header = wayland_protocols.get_pkgconfig_variable('includedir')

#include 
inc = include_directories('include', protocols_server_header)

# Sources
#shm_src = 'src/shm.c'
src_server = files('src/server.c')
#Executable: server
executable('junowm', src_server,
  include_directories: inc,
  dependencies: [wayland_server, wayland_protocols, wlroots]
)

#src_client = files('src/client.c')
#executable('testclient', src_client, dependencies: [wayland_client])


#Executable: client
#executable('testclient',['src/client.c', shm_src], dependencies: [wayland_client])
