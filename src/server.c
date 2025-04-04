#include "include/utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <wayland-server-core.h>
#include <wayland-server.h>
#include <wayland-util.h>
#include <wlroots-0.19/wlr/backend.h>

struct jwm_server {
  struct wl_display *wl_display;
  struct wl_event_loop *wl_event_loop;
  struct wlr_backend *backend;
};

int main(int argc, char **argv) {
  struct jwm_server server;
  server.wl_display = wl_display_create();
  assert(server.wl_display);
  debug_message("[+] display init. success");

  server.wl_event_loop = wl_display_get_event_loop(server.wl_display);
  assert(server.wl_event_loop);
  debug_message("[+] event loop init. success");

  server.backend = wlr_backend_autocreate(server.wl_event_loop, NULL);
  assert(server.backend);
  debug_message("[+] backend via event loop init. success");
  if (!wlr_backend_start(server.backend)) {
    fprintf(stderr, "[-] Failed to start backend.");
    wl_display_destroy(server.wl_display);
    return 1;
  }

  wl_display_run(server.wl_display);
  wl_display_destroy(server.wl_display);
  return EXIT_SUCCESS;
}
