#include <unistd.h>
#ifndef SHM_H
#define SHM_H
static void randname(char *buf);
static int create_shml_file(void);
int allocate_shm_file(size_t size);
#endif
