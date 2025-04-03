#define _POSIX_C_SOURCE 200112L
#include "include/shm.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

/**
 * Generates a random 6-character suffix for shared memory names.
 * This function utilizes a pseudo-random seed by using the time
 * of the clock_gettime() function call.
 * @param buf: buffer storing suffix.
 */
static void randname(char *buf) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  long r = ts.tv_nsec ^ (getpid() * 2654435761);
  for (int i = 0; i < 6; ++i) {
    buf[i] = 'A' + (r & 15) + (r & 16) * 2;
    r >>= 5;
  }
}

static int create_shm_file(void) {
  int retries = 100;
  do {
    char name[] = "/wl_shm-XXXXXX";
    randname(name + sizeof(name) - 7);
    --retries;
    int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
    if (fd >= 0) {
      shm_unlink(name);
      return fd;
    }
  } while (retries > 0 && errno == EEXIST);
  perror("[-] Failed to create shared memory file.");
  return -1;
}

int allocate_shm_file(size_t size) {
  int fd = create_shm_file();
  if (fd < 0)
    return -1;
  int ret;
  do {
    ret = ftruncate(fd, size);
  } while (ret < 0 && errno == EINTR);
  if (ret < 0) {
    perror("[-] Failed to resize shared memory.");
    close(fd);
    return -1;
  }
  return fd;
}
