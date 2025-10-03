#include <stdio.h>

#include "error.h"

Err_t print_error_internal(Err_t code, const char *func, const char *file, int line) {
  fprintf(stderr, "\n<!> (%s:%d, %s): ", file, line, func);

  switch (code) {
    case ERR_OK: {
      fprintf(stderr, "Success\n");
      break;
    }
    case ERR_NULL: {
      fprintf(stderr, "NULL pointer error\n");
      break;
    }
    case ERR_MEM: {
      fprintf(stderr, "Allocation error\n");
      break;
    }
    case ERR_EOF: {
      break;
    }
    case ERR_KEY_EXISTS: {
      fprintf(stderr, "Key already exists\n");
      break;
    }
    case ERR_KEY_NOT_FOUND: {
      fprintf(stderr, "Element not found\n");
      break;
    }
    case ERR_EMPTY: {
      fprintf(stderr, "Storage is empty\n");
      break;
    }
    case ERR_FULL: {
      fprintf(stderr, "Storage is full\n");
      break;
    }
    case ERR_FORMAT: {
      fprintf(stderr, "Invalid format\n");
      break;
    }
    case ERR_FILE: {
      fprintf(stderr, "File opening error\n");
      break;
    }
    case ERR_RELEASE_NOT_FOUND: {
      fprintf(stderr, "Release not found\n");
      break;
    }
    default: {
      fprintf(stderr, "Command failed or returned non-zero: %d", code);
      break;
    }
  }

  return ERR_OK;
}