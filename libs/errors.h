#ifndef __errors_h
#define __errors_h

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

enum {
  BadArgumentError = -1001,
  OverflowError,
  MemoryAllocationError,
  FileOpenError,
  RegexCompError,
  RegexExecError,
  NotFoundError,
};

int32_t error_handling(int32_t code, FILE *ostream);

#endif // __errors_h
