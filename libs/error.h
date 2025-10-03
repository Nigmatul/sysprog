#ifndef _ERROR_H
#define _ERROR_H

typedef enum {
    ERR_OK,
    ERR_NULL,
    ERR_MEM,
    ERR_EOF,
    ERR_KEY_EXISTS,
    ERR_KEY_NOT_FOUND,
    ERR_EMPTY,
    ERR_FULL,
    ERR_FORMAT,
    ERR_FILE,
    ERR_RELEASE_NOT_FOUND,
} Err_t; 

// Внутренняя функция (не должна вызываться напрямую)
Err_t print_error_internal(Err_t code, const char *func, const char *file, int line);

// Макрос для удобного использования
#define print_error(code) print_error_internal((code), __func__, __FILE__, __LINE__)

#endif
