#ifndef __myio_h
#define __myio_h

#include <inttypes.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bool.h"
#include "errors.h"

#define BUFSIZE 100
char *readline(FILE *stream, const char *prompt);

int32_t fget_int(FILE *stream, int64_t *x, const char *prompt,
                 int32_t (*filter)(int64_t x));

/*----FILTERS----*/
int32_t check_bool(int64_t x);
int32_t check_unsigned(int64_t x);
int32_t check_line(const char *line, const char *pattern);
/*---------------*/

#endif // __myio_h
