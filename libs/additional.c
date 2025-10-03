#include <sys/sysinfo.h>
#include <sys/stat.h>

#include "additional.h"

unsigned long get_available_memory_bytes() {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        return info.freeram * info.mem_unit;
    }
    return 0;
}

off_t get_file_size_bytes(int fd) {
  struct stat st;
  if (fstat(fd, &st) == 0) {
    return st.st_size;
  } else {
    return -1;
  }
}