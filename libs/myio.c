#include "myio.h"

#define STR_(X) #X
#define STR(X) STR_(X)
char *readline(FILE *stream, const char *prompt) {
  uint64_t str_len = 0;
  int32_t status = 0;

  char *str = (char *)calloc(1, sizeof(char));
  char *tmp = (char *)calloc(BUFSIZE + 1, sizeof(char));

  if (prompt != NULL) {
    printf("%s", prompt);
  }

  fscanf(stream, "%*[\n]");
  do {
    status = fscanf(stream, "%" STR(BUFSIZE) "[^\n]", tmp);
    if (status == 1) {
      str_len += strlen(tmp);
      str = (char *)realloc(str, (str_len + strlen(tmp) + 1) * sizeof(char));
      strcat(str, tmp);
    } else if (status == EOF && str_len != 0) {
      status = 1;
      clearerr(stream);
    }
  } while (status == 1);

  free(tmp);
  if (status == EOF) {
    if (stream == stdin) {
      printf("\n");
    }
    free(str);
    return NULL;
  }
  fscanf(stream, "%*c");
  return str;
}