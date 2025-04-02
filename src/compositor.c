#include <stdio.h>
#include <string.h>
#include <wayland-server.h>

struct my_compositor {
  struct wl_resource *resource;
  struct my_compositor_state *state;
  struct my_compositor *next;
};

struct my_compositor_state {
  struct my_compositor *client_compositors;
  int test;
};
