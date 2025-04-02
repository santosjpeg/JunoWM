#include <stdio.h>
#include <stdlib.h>
#include <wayland-server.h>

// Group of globals are structured in a Linked List
struct my_output {
  struct wl_resource *resource;
  struct my_state *state;
  struct my_output *next;
};

struct my_state {
  struct my_output *client_outputs;
  int test;
};

// Helper functions for list management (e.g., add and remove nodes)
void add_to_list(struct my_output **head, struct my_output *new_output) {
  new_output->next = *head;
  *head = new_output;
}

void remove_to_list(struct my_output **head, struct my_output *output) {
  struct my_output **curr = head;
  while (*curr) {
    if (*curr == output) {
      *curr = output->next;
      free(output);
      return;
    }
    curr = &(*curr)->next;
  }
}

// Handles destruction of a Wayland resource
static void wl_output_handle_resource_destroy(struct wl_resource *resource) {
  struct my_output *client_output = wl_resource_get_user_data(resource);
  if (!client_output)
    return;

  remove_to_list(&client_output->state->client_outputs, client_output);
}

// Called when client sends "release" request
static void wl_output_handle_release(struct wl_client *client,
                                     struct wl_resource *resource) {
  wl_resource_destroy(resource);
}

// struct such that ^ is the destructor
static const struct wl_output_interface wl_output_implementation = {
    .release = wl_output_handle_release,
};

// Handles client requests and binds them to wl_output interface
// @PARAM client -> client requesting to bind to wl_output interface
// @PARAM data -> pointing to my_state struct
// @PARAM version -> Wayland protocol version
// @PARAM id -> For newly created resource
// OUTPUT: Registers new resource as a global
static void wl_output_handle_bind(struct wl_client *client, void *data,
                                  uint32_t version, uint32_t id) {
  // snapshot of compositor when wl_global_create() was called
  struct my_state *state = data;

  // Tracks client's interaction with wl_output
  struct my_output *client_output = calloc(1, sizeof(struct my_output));
  if (!client_output) {
    fprintf(stderr, "Error: Failed to allocate memory for my_output.\n");
    return;
  }

  // Initializes and adds resource to globals
  struct wl_resource *resource =
      wl_resource_create(client, &wl_output_interface, version, id);
  if (!resource) {
    fprintf(stderr, "Error: Failed to create wl_resource.\n");
    free(client_output);
    return;
  }

  wl_resource_set_implementation(resource, &wl_output_implementation,
                                 client_output,
                                 wl_output_handle_resource_destroy);

  client_output->resource = resource;
  client_output->state = state;

  // TODO: Send geometry event, et al.

  wl_output_send_geometry(resource, 0, 0, 1920, 1080,
                          WL_OUTPUT_SUBPIXEL_UNKNOWN, "Foobar, Inc.",
                          "Foobar Monitor", WL_OUTPUT_TRANSFORM_NORMAL);

  // Result: Another global that is a means of a client to interact with
  // wl_display (global interface)
  add_to_list(&state->client_outputs, client_output);
}

int main(int argc, char **argv) {
  struct wl_display *display = wl_display_create();
  if (!display) {
    fprintf(stderr, "Error: Unable to create Wayland display.\n");
    return EXIT_FAILURE;
  }

  const char *socket = wl_display_add_socket_auto(display);
  if (!socket) {
    fprintf(stderr, "Error: Unable to add socket to Wayland display.\n");
    wl_display_destroy(display);
    return EXIT_FAILURE;
  }

  struct my_state state = {.client_outputs = NULL, .test = 1};

  wl_global_create(display, &wl_output_interface, 1, &state,
                   wl_output_handle_bind);
  // wl_global-create(display, &wl_compositor_interface, 1, &state,
  // wl_compositor_handle_bind);

  fprintf(stderr, "Running Wayland display on socket: %s\n", socket);
  wl_display_run(display);

  wl_display_destroy(display);
  return EXIT_SUCCESS;
}
