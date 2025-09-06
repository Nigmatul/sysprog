#include "errors.h"

int32_t error_handling(int32_t code, FILE *ostream) {
  if (ostream == NULL) {
    ostream = stderr;
  }

  switch (code) {
  case BadArgumentError: {
    fprintf(ostream, "\n<ERROR> Bad argument passed\n");
    break;
  }
  case OverflowError: {
    fprintf(ostream, "\n<ERROR> Memory overflow\n");
    break;
  }
  case MemoryAllocationError: {
    fprintf(ostream, "\n<ERROR> Couldn't allocate memory\n");
    break;
  }
  case FileOpenError: {
    fprintf(ostream, "\n<ERROR> File opening failure\n");
    break;
  }
  case RegexCompError: {
    fprintf(ostream, "\n<ERROR> Regex compilation error\n");
    break;
  }
  case RegexExecError: {
    fprintf(ostream, "\n<ERROR> Regex execution error\n");
    break;
  }
  case NotFoundError: {
    fprintf(ostream, "\n<ERROR> Element not found\n");
    break;
  }
  case EOF: {
    break;
  }
  case EXIT_SUCCESS: {
    break;
  }
  default: {
    fprintf(ostream, "<ERROR> Command failed or returned non-zero: %d", code);
    break;
  }
  }

  return EXIT_SUCCESS;
}
