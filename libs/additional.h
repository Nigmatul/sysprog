#ifndef __ ADDITIONAL_H
#define __ADDITIONAL_H

#include <sys/types.h>

#define MAX(A, B) ((A) > (B) ? (A) : (B))

unsigned long get_available_memory_bytes();
off_t get_file_size_bytes(int fd);

#endif // __ADDITIONAL_H