//#include "../include/syscalls.h"
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <xxhash.h>

#define MAX(A, B) ((A) > (B) ? (A) : (B))

typedef struct {
  uint64_t hash[2];
  size_t offset;
  size_t len;
} line_info_t;

typedef struct {
  size_t size;
  line_info_t *data;
} vector_t;

typedef struct {
  const char *data;
  size_t size;
} mapped_file_t;

void hash128(const void *ptr, size_t len, uint64_t out[2]) {
    XXH128_hash_t h = XXH3_128bits(ptr, len);
    out[0] = h.low64;
    out[1] = h.high64;
}

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

int map(const char *filename, mapped_file_t *mf) {
  if (!filename || !mf) {
    return EINVAL;
  }

  int fd = -1;
  off_t size = 0;
  const char * data = NULL;

  *mf  = (mapped_file_t){ NULL, 0 };

  fd = open(filename, O_RDONLY);
  if (fd == -1) {
      return errno;
  }

  size = get_file_size_bytes(fd);
  if (size < 0) {
      close(fd);
      return ENOMEM;
  }

  if (size == 0) {
      close(fd);
      return 0;
    }

  if (size > get_available_memory_bytes() / 4) {
      close(fd);
      return ENOMEM;
  }

  data = mmap(NULL, (size_t) size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (data == MAP_FAILED) {
    int e = errno;
    close(fd);
    return e;
  }

  close(fd);
  *mf = (mapped_file_t){ data, size };
  return 0;
}

int unmap(mapped_file_t *mf) {
    if (!mf) { 
      return EINVAL;
    }

    if (mf->data && mf->size > 0 && mf->data != MAP_FAILED) {
        if (munmap(mf->data, mf->size) == -1) {
            mf->data = NULL;
            mf->size = 0;
        }
    }

    mf->data = NULL;
    mf->size = 0;
    return 0;
}

vector_t* split_into_lines(mapped_file_t *mf) {
  if (!mf) {
    return NULL;
  }

  if (!mf->data || mf->size == 0) {
    vector_t *vec = calloc(1, sizeof(vector_t));
    if (!vec) return NULL;
    vec->size = 0;
    vec->data = NULL;
    return vec;
  }

  size_t cnt = 0;
  for (size_t i = 0; i < mf->size; ++i) {
    if (mf->data[i] == '\n') cnt++;
  }

  if (mf->size > 0 && mf->data[mf->size - 1] != '\n') {
    cnt++;
  }

  vector_t *vec = calloc(1, sizeof(vector_t));
  if (!vec) {
    return NULL;
  }

  line_info_t *lines = calloc(cnt, sizeof(line_info_t));
  if (!lines) {
    free(vec);
    return NULL;
  }

  size_t idx = 0;
  size_t start = 0;
  for (size_t i = 0; i < mf->size; ++i) {
    if (mf->data[i] == '\n') {
      size_t len = i - start + 1;
      hash128(mf->data + start, len, lines[idx].hash);
      lines[idx].offset = start;
      lines[idx].len = len;
      start = i + 1;
      idx++;
    }
  }

  if (start < mf->size) {
    size_t len = mf->size - start;
    hash128(mf->data + start, len, lines[idx].hash);
    lines[idx].offset = start;
    lines[idx].len = len;
    idx++;
  }

  vec->size = cnt;
  vec->data = lines;

  return vec;
}

void diff(const char *filename1, const char *filename2) {
  if(!filename1 || !filename2) {
    return;
  }

  int rc = 0;
  mapped_file_t mf1, mf2;

  rc = map(filename1, &mf1);
  if (rc != 0) {
    fprintf(stderr, "Ошибка при map: %s\n", strerror(rc));
    goto cleanup;
  }

  rc = map(filename2, &mf2);
  if (rc != 0) {
    fprintf(stderr, "Ошибка при map: %s\n", strerror(rc));
    goto cleanup;
  }





cleanup:
  unmap(&mf1);
  unmap(&mf2);
}