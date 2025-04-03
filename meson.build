project('junowm','c')

# Dependencies
wayland_server = dependency('wayland-server')
wayland_client = dependency('wayland-client')

# Sources
shm_src = 'src/shm.c'

#Executable: server
executable('junowm','src/server.c', dependencies: [wayland_server])

#Executable: client
executable('testclient',['src/client.c', shm_src], dependencies: [wayland_client])
