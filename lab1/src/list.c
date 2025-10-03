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

void clean(list_t *lst) {
  if (!lst) {
    return;
  }

  node_t *prev = NULL;
  node_t *curr = lst->head;
  while (curr) {
    prev = curr;
    curr = curr->next;
    free(prev);
  }

  lst->head = NULL;
  lst->size = 0;
}


void push_front(list_t *lst, line_info_t *l, char s) {
  if (!lst) {
    return;
  }

  node_t *new = calloc(1, sizeof(node_t));
  if (!new) {
    return;
  }

  new->line = l;
  new->status = s;
  new->next = lst->head;

  lst->head = new;
  lst->size++;

  return;
}