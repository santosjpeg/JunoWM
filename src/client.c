#include "include/shm.h"
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <wayland-client-protocol.h>
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

  // Initialize shared memory pool after binding to wl_shm interface

  const int WIDTH = 1920;
  const int HEIGHT = 1080;

  const int STRIDE = WIDTH * 4;
  const int SHM_POOL_SIZE = HEIGHT * STRIDE * 2;

  // Initialize and dynamically allocated size of shared memory file
  int fd = allocate_shm_file(SHM_POOL_SIZE);
  uint8_t *pool_data =
      mmap(NULL, SHM_POOL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  struct wl_shm *shm = state.shm;

  // Init wl_shm_pool object to be shared memory backing for buffer objects
  struct wl_shm_pool *pool = wl_shm_create_pool(shm, fd, SHM_POOL_SIZE);

  wl_display_roundtrip(display);
  return 0;
}
