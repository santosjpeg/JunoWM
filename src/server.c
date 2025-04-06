#include <assert.h>
#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <wayland-server-core.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/render/wlr_renderer.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_keyboard.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_pointer.h>
#include <wlr/types/wlr_scene.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_subcompositor.h>
#include <wlr/types/wlr_xcursor_manager.h>
#include <wlr/util/log.h>
#include <xkbcommon/xkbcommon.h>

#include <stdlib.h>
#include <wayland-client.h>
#include <wayland-util.h>
#include <wlr/backend.h>
#include <wlr/render/allocator.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/util/log.h>

struct jwm_server {
  struct wl_display *wl_display;
  struct wl_event_loop *wl_event_loop;
  struct wlr_backend *backend;
  struct wlr_renderer *renderer;
  struct wlr_allocator *allocator;
  struct wlr_scene *scene;
  struct wlr_scene_output_layout *scene_layout;

  struct wl_list outputs;
  struct wl_listener new_output;
};

struct jwm_output {
  struct wl_list link;
  struct jwm_server *server;
  struct wlr_output *wlr_output;
  struct wl_listener frame;
  struct wl_listener request_state;
  struct wl_listener destroy;
};

static void output_frame(struct wl_listener *listener, void *data);

static void output_request_state(struct wl_listener *listener, void *data);

static void output_destroy(struct wl_listener *listener, void *data);

static void server_new_output(struct wl_listener *listener, void *data) {
  fprintf(stderr, "[+] Running server_new_output\n");

  struct jwm_server *server = wl_container_of(listener, server, new_output);
  struct wlr_output *wlr_output = data;

  wlr_output_init_render(wlr_output, server->allocator, server->renderer);

  /* Force enable output on*/
  struct wlr_output_state state;
  wlr_output_state_init(&state);
  wlr_output_state_set_enabled(&state, true);

  /* Backends like Wayland do not have modes, which is the dimensions and
   * refresh rate of the display.*/
  struct wlr_output_mode *mode = wlr_output_preferred_mode(wlr_output);
  if (!mode) {
    wlr_output_state_set_mode(&state, mode);
  }

  /* Setting wlr_output will be applied after calling commit_state(); finish to
   * deallocate the state of the given output from memory*/
  wlr_output_commit_state(wlr_output, &state);
  wlr_output_state_finish(&state);

  /* Initialize listeners for frame, state request, and destroy events*/
  struct jwm_output *output = calloc(1, sizeof(*output));
  output->wlr_output = wlr_output;
  output->server = server;

  output->frame.notify = output_frame;
  wl_signal_add(&wlr_output->events.frame, &output->frame);

  output->request_state.notify = output_request_state;
  wl_signal_add(&wlr_output->events.request_state, &output->request_state);

  output->destroy.notify = output_destroy;
  wl_signal_add(&wlr_output->events.destroy, &output->destroy);

  wl_list_insert(&server->outputs, &output->link);
}

int main(int argc, char **argv) {
  /*Initializes the logger with minimum verbosity of INFO*/
  wlr_log_init(WLR_INFO, NULL);

  /*Initialize JunoWM server*/
  struct jwm_server server = {0};

  /* display managed by libwayland; takes care of accepting clients via UNIX
   * sockets and manages the server's globals.*/
  server.wl_display = wl_display_create();
  wlr_log(WLR_INFO, "[+] display init. success");

  server.wl_event_loop = wl_event_loop_create();

  /* Based on the current environment, wlroots automatically chooses the best
   * backend for the currently running machine. */
  server.backend = wlr_backend_autocreate(server.wl_event_loop, NULL);
  if (!wlr_backend_start(server.backend)) {
    wlr_log(WLR_ERROR, "[-] Failed to start backend.");
    wl_display_destroy(server.wl_display);
    return 1;
  }
  wlr_log(WLR_INFO, "[+] backend via event loop init. success");

  /* Initializes the renderer via autocreate() function from wlroots; the
   * renderer is responsible for the pixel format it supports for shm*/
  server.renderer = wlr_renderer_autocreate(server.backend);
  if (!server.renderer) {
    wlr_log(WLR_ERROR, "[-] Failed to initialize renderer");
    return 1;
  }
  wlr_log(WLR_INFO, "[+] init. renderer success");

  wlr_renderer_init_wl_display(server.renderer, server.wl_display);

  /* Autocreates the allocator that is responsible for creating buffer pools for
   * wlroots to render onto the screen.*/
  server.allocator = wlr_allocator_autocreate(server.backend, server.renderer);
  if (!server.allocator) {
    wlr_log(WLR_ERROR,
            "[-] Failed to initialize allocato responsible for buffers");
    return 1;
  }
  wlr_log(WLR_INFO, "[+] init. allocator success");

  /* Compositor: the meta interface that allocates surfaces onto a display.
   * Recall that a compositor is a piece of software to draw application windows
   * onto a given output.*/
  // wlr_compositor_create(server.wl_display, 5, server.renderer);
  // wlr_log(WLR_INFO, "[+] init. compositor success");

  /* Configure a listener to notify when a new output is detected (e.g., when a
   * new monitor is plugged in)*/
  wl_list_init(&server.outputs);
  server.new_output.notify = server_new_output;
  wl_signal_add(&server.backend->events.new_output, &server.new_output);
  wlr_log(WLR_INFO, "[+] init. listener for new outputs success");

  if (!wlr_backend_start(server.backend)) {
    wlr_backend_destroy(server.backend);
    wl_display_destroy(server.wl_display);
    return 1;
  }

  const char *socket = wl_display_add_socket_auto(server.wl_display);
  if (!socket) {
    wlr_log(WLR_ERROR, "[-] Failed to create UNIX socket");
    wl_display_destroy(server.wl_display);
    return 1;
  }
  wlr_log(WLR_INFO, "Running Wayland display on WAYLAND_DISPLAY=%s", socket);

  wlr_log(WLR_INFO, "[+] running display...");
  wl_display_run(server.wl_display);

  /* Manual garbage collection of all the components of the compositor...*/
  wlr_allocator_destroy(server.allocator);
  wlr_renderer_destroy(server.renderer);
  wlr_backend_destroy(server.backend);
  wl_display_destroy(server.wl_display);
  return EXIT_SUCCESS;
}
