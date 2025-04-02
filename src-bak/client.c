#include <stdio.h>
#include <wayland-client.h>

static void registry_handle_global(void *data, struct wl_registry *registry,
                                   uint32_t name, const char *interface,
                                   uint32_t version) {
  printf("interface: '%s', version: '%d', name: '%d'\n", interface, version,
         name);
}

static void registry_handle_global_remove(void *data,
                                          struct wl_registry *registry,
                                          uint32_t name) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove,
};

int main(int argc, char **argv) {
  // Initialize display and registry

  // display: wire protocol
  // TYPE: request
  // object ID: 1
  // message length: 12 bytes
  // opcode: 1
  struct wl_display *display = wl_display_connect(NULL);
  if (!display) {
    fprintf(stderr, "Failed to create Wayland display.");
    return 1;
  }

  // display: wire protocol
  // TYPE: event
  // object ID: 2
  // message length: 13 bytes
  // opcode: 0
  struct wl_registry *registry = wl_display_get_registry(display);
  if (!registry) {
    fprintf(stderr, "Failed to create Wayland registry.");
    return 1;
  }
  wl_registry_add_listener(registry, &registry_listener, NULL);
  wl_display_roundtrip(display);
  return 0;
}
