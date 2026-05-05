#ifndef SLATE_RUNTIME_H
#define SLATE_RUNTIME_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
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

static inline int64_t slate_file_exists(char* path) {
    FILE* f = fopen(path, "r");
    if (f == NULL) return 0;
    fclose(f);
    return 1;
}

static inline int64_t slate_len(void* list) {
    if (list == NULL) return 0;
    void*** arr = (void***)list;
    return (int64_t)arr[1];
}

static inline void* slate_get(void* list, int64_t idx) {
    void*** arr = (void***)list;
    return arr[0][idx];
}

static inline void slate_set(void* list, int64_t idx, void* val) {
    void*** arr = (void***)list;
    arr[0][idx] = val;
}

static inline void* slate_add(void* list, void* val) {
    return list;
}

static inline int64_t slate_has(void* table, void* key) {
    return 0;
}

static inline void* slate_keys(void* table) {
    return NULL;
}

static inline void* slate_values(void* table) {
    return NULL;
}

static inline char* slate_split(char* s, char* delim) {
    return strdup(s);
}

static inline int64_t slate_contains(char* s, char* sub) {
    if (s == NULL || sub == NULL) return 0;
    return strstr(s, sub) != NULL;
}

static inline char* slate_trim(char* s) {
    if (s == NULL) return NULL;
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') s++;
    char* end = s + strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) end--;
    char* result = malloc((size_t)(end - s + 2));
    strncpy(result, s, (size_t)(end - s + 1));
    result[end - s + 1] = 0;
    return result;
}

static inline char* slate_replace(char* s, char* old, char* new) {
    if (s == NULL) return NULL;
    return strdup(s);
}

static inline int64_t slate_starts_with(char* s, char* prefix) {
    if (s == NULL || prefix == NULL) return 0;
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

static inline int64_t slate_ends_with(char* s, char* suffix) {
    if (s == NULL || suffix == NULL) return 0;
    size_t slen = strlen(s);
    size_t slen2 = strlen(suffix);
    if (slen < slen2) return 0;
    return strcmp(s + slen - slen2, suffix) == 0;
}

static inline int64_t slate_to_int(void* v) {
    return 0;
}

static inline double slate_to_float(void* v) {
    return 0.0;
}

static inline char* slate_to_string(void* v) {
    return "";
}

static inline char* slate_home() {
    char* home = getenv("HOME");
    if (home == NULL) return "/tmp";
    return home;
}

static inline void* slate_file(char* path) {
    return (void*)path;
}

static inline int64_t slate_run(char* cmd) {
    return system(cmd);
}

static inline int64_t slate_hash(char* s) {
    if (s == NULL) return 0;
    int64_t h = 0;
    while (*s) {
        h = h * 31 + *s;
        s++;
    }
    return h;
}

static inline char* slate_render_all(void* errors, void* warnings, char* source) {
    return "";
}

static inline void* slate_parse(char* source) {
    return NULL;
}

static inline void* slate_analyse(void* node, char* file) {
    return NULL;
}

#endif
