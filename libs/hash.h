#ifndef __HASH_H
#define __HASH_H

#include <inttypes.h>
#include <stddef.h>

void hash128(const void *ptr, size_t len, uint64_t out[2]);

#endif // __HASH_H
