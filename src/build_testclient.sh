wayland-scanner private-code \
  </usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
  >xdg-shell-client-protocol.c
wayland-scanner client-header \
  </usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
  >xdg-shell-client-protocol.h
cc -o client client.c xdg-shell-client-protocol.c -lwayland-client -lrt
