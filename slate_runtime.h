#ifndef SLATE_RUNTIME_H
#define SLATE_RUNTIME_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static inline void* slate_alloc(int64_t size) {
    return malloc((size_t)size);
}

static inline void slate_free(void* ptr) {
    free(ptr);
}

static inline int64_t slate_strlen(char* s) {
    if (s == NULL) return 0;
    return (int64_t)strlen(s);
}

static inline char* slate_strdup(char* s) {
    if (s == NULL) return NULL;
    return strdup(s);
}

static inline int64_t slate_strcmp(char* a, char* b) {
    if (a == NULL && b == NULL) return 0;
    if (a == NULL) return -1;
    if (b == NULL) return 1;
    return strcmp(a, b);
}

static inline void* slate_list_make(int64_t cap) {
    void** arr = malloc((size_t)(cap * sizeof(void*)));
    memset(arr, 0, (size_t)(cap * sizeof(void*)));
    return arr;
}

static inline void* slate_list_get(void* list, int64_t idx) {
    void*** arr = (void***)list;
    return arr[0][idx];
}

static inline void slate_list_set(void* list, int64_t idx, void* val) {
    void*** arr = (void***)list;
    arr[0][idx] = val;
}

static inline int64_t slate_time() {
    return (int64_t)time(NULL);
}

static inline char* slate_env(char* name) {
    char* val = getenv(name);
    if (val == NULL) return "";
    return val;
}

static inline void slate_print(char* s) {
    printf("%s", s);
}

static inline void slate_print_int(int64_t n) {
    printf("%lld", (long long)n);
}

static inline void slate_print_float(double f) {
    printf("%g", f);
}

static inline void slate_println() {
    printf("\n");
}

static inline char* slate_int_to_str(int64_t n) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%lld", (long long)n);
    return strdup(buf);
}

static inline char* slate_float_to_str(double f) {
    char buf[64];
    snprintf(buf, sizeof(buf), "%g", f);
    return strdup(buf);
}

static inline int64_t slate_str_to_int(char* s) {
    if (s == NULL || s[0] == 0) return 0;
    return (int64_t)atoll(s);
}

static inline double slate_str_to_float(char* s) {
    if (s == NULL || s[0] == 0) return 0.0;
    return atof(s);
}

static inline void* slate_file_read(char* path) {
    FILE* f = fopen(path, "r");
    if (f == NULL) return NULL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = malloc((size_t)len + 1);
    fread(buf, 1, (size_t)len, f);
    buf[len] = 0;
    fclose(f);
    return buf;
}

static inline int64_t slate_file_write(char* path, char* data) {
    FILE* f = fopen(path, "w");
    if (f == NULL) return 0;
    fputs(data, f);
    fclose(f);
    return 1;
}

#endif
