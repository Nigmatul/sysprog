#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <unistd.h>

#include "mapping.h"

off_t get_file_size_bytes(int fd) {
  struct stat st;
  if (fstat(fd, &st) == 0) {
    return st.st_size;
  } else {
    return -1;
  }
}

int map(mapped_file_t *mf, const char *filename, int mode) {
  if (!mf || !filename) {
    errno = EINVAL;
    return -1;
  }

  *mf  = (mapped_file_t){ NULL, 0 };
  int open_mode = (mode == READ ? O_RDONLY : O_RDWR);
  int map_mode = (mode == READ ? PROT_READ : PROT_WRITE);

  int fd = open(filename, open_mode);
  if (fd == -1) {
    return -1;
  }

  off_t size = get_file_size_bytes(fd);
  if (size < 0) {
    int save = errno;
    close(fd);
    errno = save;
    return -1;
  }

  if (size == 0) {
    close(fd);
    return 0;
  }

  void *data = mmap(NULL, (size_t) size, map_mode, MAP_SHARED, fd, 0);
  if (data == MAP_FAILED) {
    int save = errno;
    close(fd);
    errno = save;
    return -1;
  }

  close(fd);
  *mf = (mapped_file_t){ (const char *)data, size };

  return 0;
}

int unmap(mapped_file_t *mf) {
  if (!mf) { 
    errno = EINVAL;
    return -1;
  }

  int ret = 0;

  if (mf->data && mf->size > 0 && mf->data != MAP_FAILED) {
    ret = munmap((void *)mf->data, mf->size);
  }

  mf->data = NULL;
  mf->size = 0;
  return ret;
}