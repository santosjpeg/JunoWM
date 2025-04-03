#include <stdio.h>
#include <string.h>
#include <wayland-client.h>

struct our_state {
  struct our_state *state;
  struct wl_compositor *compositor;
  struct wl_shm *shm;
  struct my_output *next;
};

static void registry_handle_global(void *data, struct wl_registry *registry,
                                   uint32_t name, const char *interface,
                                   uint32_t version) {
  struct our_state *state = data;
  // If the interface from server matches the name of the global wl_compositor
  if (strcmp(interface, wl_compositor_interface.name) == 0) {
    // Object binds to wl_compositor, initializing a resource stored server-side
    state->compositor =
        wl_registry_bind(registry, name, &wl_compositor_interface, 4);
    fprintf(stderr, "[+] Successfully binded to wl_compositor.\n");
  }

  // Finds shared memory and binds to it, creating resource server-side
  if (strcmp(interface, wl_shm_interface.name) == 0) {
    state->shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
    fprintf(stderr, "[+] Successfully binded to wl_shm.\n");
  }
}

static void registry_handle_global_remove(void *data,
                                          struct wl_registry *registry,
                                          uint32_t name) {}

static const struct wl_registry_listener registry_listener = {
    .global = registry_handle_global,
    .global_remove = registry_handle_global_remove,
};

int main(int argc, char **argv) {
  struct our_state state = {0};
  // Initialize display and registry

  struct wl_display *display = wl_display_connect(NULL);
  if (!display) {
    fprintf(stderr, "Failed to create Wayland display.");
    return 1;
  }

  struct wl_registry *registry = wl_display_get_registry(display);
  if (!registry) {
    fprintf(stderr, "Failed to create Wayland registry.");
    return 1;
  }
  wl_registry_add_listener(registry, &registry_listener, &state);
  wl_display_roundtrip(display);
  return 0;
}
