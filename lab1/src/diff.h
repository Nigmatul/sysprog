#ifndef __DIFF_H
#define __DIFF_H

#include <stddef.h>
#include <inttypes.h>

/*
  TODO 
  Написать свой аллокатор
*/

typedef struct line_info{
  const char *data;
  int64_t length;
  uint64_t hash;
} line_info_t;

typedef struct vector{
  int64_t size;
  int64_t capacity;
  void *data;
} vector_t;

#endif // __DIFF_H