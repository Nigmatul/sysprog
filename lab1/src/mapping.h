#ifndef __MAPPING_H
#define __MAPPING_H

#include <stddef.h>
#include <sys/types.h>
#include <inttypes.h>

enum {
  READ, 
  WRITE
};

typedef struct mapped_file {
  const char *data;
  int64_t size;
} mapped_file_t;

off_t get_file_size_bytes(int fd);
int map(mapped_file_t *mf, const char *filename, int mode);
int unmap(mapped_file_t *mf);

#endif // __MAPPING_H