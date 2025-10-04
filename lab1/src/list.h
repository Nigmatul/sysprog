#ifndef __LIST_H
#define __LIST_H

#include <stdlib.h>
#include <stddef.h>
#include <inttypes.h>

#include "diff.h"

typedef struct node{
  line_info_t *line;
  char status; // '+', '-' or ' '
  struct node *next;
} node_t;

typedef struct {
  node_t *head;
  size_t size;
} list_t;

void clean(list_t *lst);
void push_front(list_t *lst, line_info_t *l, char s);

#endif // __LIST_H