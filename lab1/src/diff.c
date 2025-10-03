//#include "../include/syscalls.h"

#include <stddef.h>
#include <stdio.h>
#include <errno.h>
#include <xxhash.h>
#include <sys/mman.h>

#include "diff.h"
#include "mapping.h"
#include "list.c"
#include "bool.h"

static vector_t index_file(mapped_file_t *mf) {
  if (!mf) {
    errno = EINVAL;
    return (vector_t){0, NULL};
  }

  if (!mf->data || mf->size == 0) {
    errno = EINVAL;
    return (vector_t){0, NULL};
  }

  int64_t cnt = 0;
  for (int64_t i = 0; i < mf->size; ++i) {
    if (mf->data[i] == '\n') cnt++;
  }

  if (mf->size > 0 && mf->data[mf->size - 1] != '\n') {
    cnt++;
  }

  line_info_t *lines = calloc(cnt, sizeof(line_info_t));
  if (!lines) {
    return (vector_t){0, NULL};
  }

  int64_t idx = 0;
  int64_t start = 0;
  for (int64_t i = 0; i < mf->size; ++i) {
    if (mf->data[i] == '\n') {
      int64_t length = i - start + 1;

      lines[idx].hash = XXH64(mf->data + start, length, 0);
      lines[idx].length = length;
      lines[idx].data = mf->data + start;

      start = i + 1;
      idx++;
    }
  }

  if (start < mf->size) {
    int64_t length = mf->size - start;

    lines[idx].hash = XXH64(mf->data + start, length, 0);
    lines[idx].length = length;
    lines[idx].data = mf->data + start;
    idx++;
  }

  return (vector_t){cnt, lines};
}

static inline bool_t line_equals(line_info_t *l1, line_info_t *l2) {
    if (l1->hash != l2->hash) {
      return False;
    }
    
    if (l1->length != l2->length) {
      return False;
    }

    return memcmp(l1->data, l2->data, l1->length) == 0;
}

list_t myers_diff(vector_t *f1, vector_t *f2) {
  int64_t size1 = f1->size;
  line_info_t *data1 = (line_info_t *)f1->data;
  
  int64_t size2 = f2->size;
  line_info_t *data2 = (line_info_t *)f2->data;

  int64_t maxd = size1 + size2;

  vector_t trace;

  trace.size = 0;
  trace.capacity = (maxd + 1) * (maxd + 1);
  trace.data = calloc(trace.capacity, sizeof(int64_t));
  if (!trace.data) {
    return (list_t){0, NULL};
  }

  int64_t *V = (int64_t *)trace.data;

  for (int64_t d = 0; d <= maxd; ++d) {
    trace.size++;
    for (int64_t k = -d; k <= d; k += 2) {
      int64_t base = d * (d + 1);

      int64_t x;
      if (k == -d || (k != d && V[base + (k - 1)] < V[base + (k + 1)])) {
        x = V[base + (k + 1)];
      } else {
        x = V[base + (k - 1)] + 1;
      }

      int64_t y = x - k;

      while (x < size1 && y < size2 && line_equals(&data1[x], &data2[y])) {
        x++;
        y++;
      }

      V[base + k] = x;
      if (x >= size1 && y >= size2) {
        list_t res = backtrack(f1, f2, &trace);
        free(V);
        return res;
      }
    }

    if (d != maxd) {
      int64_t start = d * d;
      int64_t end = (d + 1) * (d + 1);
      for (int32_t i = start, j = end + 1; i < end; i++, j++) {
        V[j] = V[i];
      } 
    }
  }

  free(V);
  return (list_t){0, NULL};
}

static list_t backtrack(vector_t *f1, vector_t *f2, vector_t *trace) {
  if (!f1 || !f2 || !trace) {
    return (list_t){0, NULL};
  }

  list_t lst = {NULL, 0};
  int64_t x = f1->size;
  int64_t y = f2->size;
  int64_t *V = trace->data;

  for (int64_t d = trace->size - 1; x > 0 || y > 0; --d) {
    int32_t k = x - y;
    int64_t base = d * (d + 1);

    int64_t prev_k;
    if (k == -d || (k != d && V[base + (k - 1)] < V[base + (k + 1)])) {
      prev_k = k + 1;
    } else {
      prev_k = k - 1;
    }

    uint32_t prev_x = V[base + prev_k];
    uint32_t prev_y = prev_x - prev_k;

    while (x > prev_x && y > prev_y) {
      push_front(&lst, &f1->data[x - 1], ' '); // совпадение
      x--;
      y--;
    }

    if (x == prev_x) {
      push_front(&lst, &f2->data[y - 1], '+');
      y--;
    } else {
      push_front(&lst, &f1->data[x - 1], '-');
      x--;
    }
  
  }

  return lst;
}

void print_diff_list(const list_t *lst) {
    if (!lst) return;

    node_t *cur = lst->head;
    while (cur) {
        line_info_t *line = cur->line;
        if (!line) {
            cur = cur->next;
            continue;
        }

        // Печатаем статус в начале строки, как делает diff
        putchar(cur->status);

        // Печатаем саму строку
        fwrite(line->data, 1, line->length, stdout);

        // Если строка не заканчивается переводом — добавим его вручную,
        // чтобы вывод не «слипался».
        if (line->length == 0 || line->data[line->length - 1] != '\n') {
            putchar('\n');
        }

        cur = cur->next;
    }
}

void diff(const char *filename1, const char *filename2) {
  if(!filename1 || !filename2) {
    return;
  }

  int rc = 0;
  mapped_file_t mf1, mf2;

  rc = map(&mf1, filename1, READ);
  if (rc != 0) {
    fprintf(stderr, "Ошибка при map: %s\n", strerror(rc));
    goto cleanup;
  }

  rc = map(&mf2, filename2, READ);
  if (rc != 0) {
    fprintf(stderr, "Ошибка при map: %s\n", strerror(rc));
    goto cleanup;
  }

  vector_t indexed1 = index_file(&mf1);
  vector_t indexed2 = index_file(&mf2);

  list_t dff = myers_diff(&indexed1, &indexed2);

  print_diff_list(&dff);


  free(indexed1.data);
  free(indexed2.data);
  clean(&dff);

cleanup:
  unmap(&mf1);
  unmap(&mf2);
}

int main(void) {
    char file1[1024];
    char file2[1024];

    printf("Введите имя первого файла: ");
    if (!fgets(file1, sizeof(file1), stdin)) {
        fprintf(stderr, "Ошибка ввода\n");
        return 1;
    }

    printf("Введите имя второго файла: ");
    if (!fgets(file2, sizeof(file2), stdin)) {
        fprintf(stderr, "Ошибка ввода\n");
        return 1;
    }

    // Удалим возможные переводы строки
    file1[strcspn(file1, "\n")] = '\0';
    file2[strcspn(file2, "\n")] = '\0';

    // Проверим, что имена не пустые
    if (file1[0] == '\0' || file2[0] == '\0') {
        fprintf(stderr, "Имена файлов не могут быть пустыми\n");
        return 1;
    }

    diff(file1, file2);
    return 0;
}