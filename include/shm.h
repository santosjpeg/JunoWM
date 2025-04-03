#include <unistd.h>
#ifndef SHM_H
#define SHM_H
void randname(char *buf);
int create_shml_file(void);
int allocate_shm_file(size_t size);
#endif
