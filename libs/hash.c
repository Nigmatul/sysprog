#include <xxhash.h>

#include "hash.h"

void hash128(const void *ptr, size_t len, uint64_t out[2]) {
    XXH128_hash_t h = XXH3_128bits(ptr, len);
    out[0] = h.low64;
    out[1] = h.high64;
}
