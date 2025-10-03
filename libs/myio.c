#include "myio.h"

#define STR_(X) #X
#define STR(X) STR_(X)
char *readline(FILE *stream, const char *prompt) {
  uint64_t str_len = 0;
  int32_t status = 0;

  char *str = (char *)calloc(1, sizeof(char)),
       *tmp = (char *)calloc(BUFSIZE + 1, sizeof(char));

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

int32_t fget_int(FILE *stream, int64_t *x, const char *prompt,
                 int32_t (*filter)(int64_t x)) {

  if (stream == NULL) {
    return BadArgumentError;
  }

  int32_t assigned = False;

  while (!assigned) {
    if (prompt != NULL && stream == stdin) {
      fprintf(stdout, "%s", prompt);
    }

    int32_t input = fscanf(stream, "%17ld", x);
    fscanf(stream, "%*[^ \t\n]");
    int32_t flag = (filter == NULL ? True : filter(*x));

    if (input == EOF) {
      return EOF;
    } else if (input == 1 && flag) {
      assigned = True;
    } else if (input == 0 || !flag) {
      fprintf(stderr, "\n<Error> Incorrect input. Try again!\n");
      fscanf(stream, "%*[^\n]");
    }
  }

  return EXIT_SUCCESS;
}

int32_t check_bool(int64_t x) { return (x == False || x == True); }

int32_t check_unsigned(int64_t x) { return (x >= 0); }

int32_t check_line(const char *line, const char *pattern) {
  regex_t exp;
  int32_t code = regcomp(&exp, pattern, REG_EXTENDED);
  if (code != EXIT_SUCCESS) {
    regfree(&exp);
    return RegexCompError;
  }

  code = regexec(&exp, line, 0, NULL, 0);
  if (code != EXIT_SUCCESS) {
    regfree(&exp);
    return RegexExecError;
  }

  regfree(&exp);
  return EXIT_SUCCESS;
}