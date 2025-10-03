#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "mapping.h"

void write_temp_file(const char *filename, const char *content) {
    int fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0600);
    assert(fd != -1);
    write(fd, content, strlen(content));
    close(fd);
}

void test_map_valid_file() {
    const char *filename = "test_file.txt";
    const char *content = "This is test content.";
    write_temp_file(filename, content);

    mapped_file_t mf;
    int res = map(&mf, filename, PROT_READ);
    assert(res == 0);
    assert(mf.data != NULL);
    assert(mf.size == strlen(content));
    assert(memcmp(mf.data, content, mf.size) == 0);

    assert(unmap(&mf) == 0);
    remove(filename);

    printf("✅ test_map_valid_file passed\n");
}

void test_map_empty_file() {
    const char *filename = "empty_file.txt";
    write_temp_file(filename, ""); // create empty file

    mapped_file_t mf;
    int res = map(&mf, filename, PROT_READ);
    assert(res == 0);
    assert(mf.data == NULL);
    assert(mf.size == 0);

    assert(unmap(&mf) == 0);
    remove(filename);

    printf("✅ test_map_empty_file passed\n");
}

void test_map_null_args() {
    mapped_file_t mf;

    errno = 0;
    assert(map(NULL, "file.txt", PROT_READ) == -1);
    assert(errno == EINVAL);

    errno = 0;
    assert(map(&mf, NULL, PROT_READ) == -1);
    assert(errno == EINVAL);

    printf("✅ test_map_null_args passed\n");
}

void test_map_nonexistent_file() {
    mapped_file_t mf;

    int res = map(&mf, "no_such_file.txt", PROT_READ);
    assert(res == -1);
    assert(errno == ENOENT);

    printf("✅ test_map_nonexistent_file passed\n");
}

void test_unmap_valid() {
    const char *filename = "file_to_unmap.txt";
    const char *content = "Unmap this.";

    write_temp_file(filename, content);

    mapped_file_t mf;
    assert(map(&mf, filename, PROT_READ) == 0);
    assert(mf.data != NULL);

    int res = unmap(&mf);
    assert(res == 0);
    assert(mf.data == NULL);
    assert(mf.size == 0);

    remove(filename);

    printf("✅ test_unmap_valid passed\n");
}

void test_unmap_null() {
    errno = 0;
    assert(unmap(NULL) == -1);
    assert(errno == EINVAL);

    printf("✅ test_unmap_null passed\n");
}

void test_unmap_without_map() {
    mapped_file_t mf = { NULL, 0 };
    int res = unmap(&mf);
    assert(res == 0); // unmap без данных не должен падать

    printf("✅ test_unmap_without_map passed\n");
}

#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "mapping.h"

// Безопасная запись всего буфера в файл
ssize_t write_all(int fd, const void *buf, size_t count) {
    const char *ptr = buf;
    size_t total_written = 0;
    while (total_written < count) {
        ssize_t written = write(fd, ptr + total_written, count - total_written);
        if (written < 0) return -1;
        if (written == 0) break; // shouldn't happen
        total_written += written;
    }
    return total_written;
}

void test_map_large_file(void) {
    const char *filename = "test_large_file.bin";
    size_t size = 20ULL * 1024 * 1024 * 1024; // 5 GB

    // 1. Создание и запись файла
    int fd = open(filename, O_CREAT | O_RDWR, 0644);
    assert(fd != -1);

    // 2. Установка размера файла
    assert(ftruncate(fd, size) == 0);

    // 3. Запись в файл
    // Буфер можно сделать меньше и писать по блокам, но тут пишем 0-буферы mmap'ом
    void *map_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    assert(map_ptr != MAP_FAILED);

    // Заполнение данных (например, единицами)
    memset(map_ptr, 1, size);

    // Явно сбросить изменения на диск
    assert(msync(map_ptr, size, MS_SYNC) == 0);

    munmap(map_ptr, size);

    // Обязательно — сброс метаданных на диск
    assert(fsync(fd) == 0);
    close(fd);

    // 4. Теперь протестируем отображение
    mapped_file_t mf;
    int res = map(&mf, filename, PROT_READ);
    assert(res == 0);
    assert(mf.size == size);
    assert(mf.data != NULL);

    // 5. Проверим содержимое (должны быть все байты == 1)
    for (size_t i = 0; i < 1024; ++i) {
        assert(mf.data[i] == 1);
    }

    // 6. Завершение
    assert(unmap(&mf) == 0);

    // Удаляем тестовый файл
    remove(filename);

    printf("✅ test_map_large_file passed\n");
}


void test_map_and_edit_file() {
    const char *filename = "editable_file.txt";
    const char *original = "Hello World!";
    const char *modified = "HELLO WORLD!";

    int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0600);
    assert(fd != -1);
    assert(write(fd, original, strlen(original)) == (ssize_t)strlen(original));
    close(fd);

    mapped_file_t mf;
    int res = map(&mf, filename, PROT_READ | PROT_WRITE);
    assert(res == 0);
    assert(mf.data != NULL);
    assert(mf.size == strlen(original));

    // Копируем в изменяемый буфер
    char *writable = (char *)mf.data;
    for (size_t i = 0; i < mf.size; ++i) {
        if (writable[i] >= 'a' && writable[i] <= 'z') {
            writable[i] -= 32; // to upper
        }
    }

    // Синхронизируем изменения с файлом
    assert(msync(writable, mf.size, MS_SYNC) == 0);

    assert(unmap(&mf) == 0);

    // Проверим, что изменения реально в файле
    char buf[64] = {0};
    fd = open(filename, O_RDONLY);
    assert(fd != -1);
    assert(read(fd, buf, sizeof(buf)) == (ssize_t)strlen(modified));
    close(fd);

    assert(strncmp(buf, modified, strlen(modified)) == 0);
    remove(filename);

    printf("✅ test_map_and_edit_file passed\n");
}

int main() {
    test_map_valid_file();
    test_map_empty_file();
    test_map_null_args();
    test_map_nonexistent_file();
    test_unmap_valid();
    test_unmap_null();
    test_unmap_without_map();
    test_map_large_file();
    test_map_and_edit_file();

    printf("\nAll tests passed!\n");
    return 0;
}
