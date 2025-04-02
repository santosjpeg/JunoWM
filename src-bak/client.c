#include <stdio.h>
#include <wayland-client.h>

int main(int argc, char **argv) {
  struct wl_display *display = wl_display_connect(NULL);
  if (!display)
    return 1;

  fprintf(stderr, "Connected!\n");
  return 0;
}
