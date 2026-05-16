#ifndef SLATE_RUNTIME_H
#define SLATE_RUNTIME_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#include <sys/wait.h>
#include <dirent.h>

// ── Memory ────────────────────────────────────────────────────────────────────

static inline void* slate_alloc(int64_t size) {
    return malloc((size_t)size);
}

// ── Strings ───────────────────────────────────────────────────────────────────

static inline char* slate_concat(const char* a, const char* b) {
    if (!a) a = "";
    if (!b) b = "";
    size_t la = strlen(a), lb = strlen(b);
    char* r = malloc(la + lb + 1);
    memcpy(r, a, la);
    memcpy(r + la, b, lb);
    r[la + lb] = 0;
    return r;
}

static inline int64_t slate_strlen(const char* s) {
    return s ? (int64_t)strlen(s) : 0;
}

static inline char* slate_str_upper(const char* s) {
    if (!s) return strdup("");
    char* r = strdup(s);
    for (char* p = r; *p; p++) *p = (char)toupper((unsigned char)*p);
    return r;
}

static inline char* slate_str_lower(const char* s) {
    if (!s) return strdup("");
    char* r = strdup(s);
    for (char* p = r; *p; p++) *p = (char)tolower((unsigned char)*p);
    return r;
}

static inline char* slate_str_reverse(const char* s) {
    if (!s) return strdup("");
    size_t len = strlen(s);
    char* r = malloc(len + 1);
    for (size_t i = 0; i < len; i++) r[i] = s[len - 1 - i];
    r[len] = 0;
    return r;
}

static inline char* slate_str_repeat(const char* s, int64_t n) {
    if (!s || n <= 0) return strdup("");
    size_t sl = strlen(s);
    char* r = malloc(sl * (size_t)n + 1);
    for (int64_t i = 0; i < n; i++) memcpy(r + sl * (size_t)i, s, sl);
    r[sl * (size_t)n] = 0;
    return r;
}

static inline char* slate_strdup(const char* s) {
    return s ? strdup(s) : NULL;
}

static inline int64_t slate_strcmp(const char* a, const char* b) {
    if (!a && !b) return 0;
    if (!a) return -1;
    if (!b) return 1;
    return (int64_t)strcmp(a, b);
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

static inline int64_t slate_to_int(void* val) {
    if (!val) return 0;
    char* s = (char*)val;
    return (int64_t)atoll(s);
}

static inline double slate_to_float(void* val) {
    if (!val) return 0.0;
    char* s = (char*)val;
    return atof(s);
}

static inline char* slate_to_string(void* val) {
    if (!val) return strdup("");
    return strdup((char*)val);
}

static inline int64_t slate_contains(const char* s, const char* sub) {
    if (!s || !sub) return 0;
    return strstr(s, sub) != NULL;
}

static inline int64_t slate_starts_with(const char* s, const char* prefix) {
    if (!s || !prefix) return 0;
    return strncmp(s, prefix, strlen(prefix)) == 0;
}

static inline int64_t slate_ends_with(const char* s, const char* suffix) {
    if (!s || !suffix) return 0;
    size_t sl = strlen(s), xl = strlen(suffix);
    if (sl < xl) return 0;
    return strcmp(s + sl - xl, suffix) == 0;
}

static inline char* slate_trim(const char* s) {
    if (!s) return strdup("");
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '\r') s++;
    const char* end = s + strlen(s);
    while (end > s && (end[-1] == ' ' || end[-1] == '\t' || end[-1] == '\n' || end[-1] == '\r')) end--;
    size_t len = (size_t)(end - s);
    char* r = malloc(len + 1);
    memcpy(r, s, len);
    r[len] = 0;
    return r;
}

static inline char* slate_replace(const char* s, const char* old, const char* replacement) {
    if (!s || !old || !replacement) return strdup(s ? s : "");
    size_t old_len = strlen(old);
    if (old_len == 0) return strdup(s);
    size_t new_len = strlen(replacement);
    // Count occurrences
    size_t count = 0;
    const char* p = s;
    while ((p = strstr(p, old))) { count++; p += old_len; }
    if (count == 0) return strdup(s);
    size_t src_len = strlen(s);
    char* result = malloc(src_len + count * (new_len - old_len) + new_len + 1);
    char* out = result;
    p = s;
    const char* found;
    while ((found = strstr(p, old))) {
        size_t prefix_len = (size_t)(found - p);
        memcpy(out, p, prefix_len);
        out += prefix_len;
        memcpy(out, replacement, new_len);
        out += new_len;
        p = found + old_len;
    }
    size_t tail = strlen(p);
    memcpy(out, p, tail);
    out[tail] = 0;
    return result;
}

static inline void* slate_split(const char* s, const char* delim) {
    // Returns a void** tagged list: [count, capacity, item0, item1, ...]
    if (!s || !delim) {
        void** r = malloc(2 * sizeof(void*));
        r[0] = (void*)(int64_t)0;
        r[1] = (void*)(int64_t)0;
        return r;
    }
    size_t dlen = strlen(delim);
    // Count parts
    size_t count = 1;
    const char* p = s;
    while ((p = strstr(p, delim))) { count++; p += dlen; }
    void** arr = malloc((count + 2) * sizeof(void*));
    arr[0] = (void*)count;
    arr[1] = (void*)count;
    p = s;
    size_t idx = 2;
    const char* found;
    while ((found = strstr(p, delim))) {
        size_t len = (size_t)(found - p);
        char* part = malloc(len + 1);
        memcpy(part, p, len);
        part[len] = 0;
        arr[idx++] = part;
        p = found + dlen;
    }
    arr[idx] = strdup(p);
    return arr;
}

// ── Collections ───────────────────────────────────────────────────────────────
// Tagged void** layout: arr[0] = (void*)count, arr[1] = (void*)capacity, arr[2..] = items

static inline int64_t slate_len(void* list) {
    if (!list) return 0;
    void** arr = (void**)list;
    return (int64_t)arr[0];
}

static inline void* slate_get(void* list, int64_t idx) {
    if (!list) return NULL;
    void** arr = (void**)list;
    int64_t count = (int64_t)arr[0];
    if (idx < 0 || idx >= count) return NULL;
    return arr[idx + 2];
}

static inline void* slate_add(void* list, void* val) {
    if (!list) {
        void** arr = malloc(6 * sizeof(void*));
        arr[0] = (void*)(int64_t)1;
        arr[1] = (void*)(int64_t)4;
        arr[2] = val;
        return arr;
    }
    void** arr = (void**)list;
    int64_t count = (int64_t)arr[0];
    int64_t cap   = (int64_t)arr[1];
    if (count < cap) {
        arr[count + 2] = val;
        arr[0] = (void*)(count + 1);
        return arr;
    }
    int64_t new_cap = cap < 4 ? 4 : cap * 2;
    void** tmp = realloc(arr, (size_t)(new_cap + 2) * sizeof(void*));
    if (!tmp) return arr;
    void** new_arr = tmp;
    new_arr[0] = (void*)(count + 1);
    new_arr[1] = (void*)new_cap;
    new_arr[count + 2] = val;
    return new_arr;
}

static inline void* slate_empty_list(void) {
    void** arr = malloc(6 * sizeof(void*));
    arr[0] = (void*)(int64_t)0;
    arr[1] = (void*)(int64_t)4;
    return arr;
}

static inline void* slate_list_first(void* list) {
    if (!list) return NULL;
    void** arr = (void**)list;
    return (int64_t)arr[0] > 0 ? arr[2] : NULL;
}

static inline void* slate_list_last(void* list) {
    if (!list) return NULL;
    void** arr = (void**)list;
    int64_t count = (int64_t)arr[0];
    return count > 0 ? arr[count + 1] : NULL;
}

static inline void* slate_list_remove(void* list, int64_t idx) {
    if (!list) return slate_empty_list();
    void** arr = (void**)list;
    int64_t count = (int64_t)arr[0];
    if (idx < 0 || idx >= count) return list;
    void** r = malloc((size_t)(count + 1) * sizeof(void*));
    r[0] = (void*)(count - 1);
    r[1] = (void*)(count - 1);
    int64_t ri = 2;
    for (int64_t i = 0; i < count; i++)
        if (i != idx) r[ri++] = arr[i + 2];
    return r;
}

static inline int _slate_strcmp_wrap(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

static inline void* slate_list_sort(void* list) {
    if (!list) return slate_empty_list();
    void** arr = (void**)list;
    int64_t count = (int64_t)arr[0];
    int64_t cap   = (int64_t)arr[1];
    void** r = malloc((size_t)(count + 2) * sizeof(void*));
    r[0] = arr[0];
    r[1] = (void*)cap;
    for (int64_t i = 0; i < count; i++) r[i + 2] = arr[i + 2];
    qsort(r + 2, (size_t)count, sizeof(void*),
          (int(*)(const void*, const void*))_slate_strcmp_wrap);
    return r;
}

static inline void* slate_list_merge(void* a, void* b) {
    if (!a) return b ? slate_empty_list() : b;
    if (!b) return slate_empty_list();
    void** arr_a = (void**)a;
    void** arr_b = (void**)b;
    int64_t ca = (int64_t)arr_a[0], cb = (int64_t)arr_b[0];
    void** r = malloc((size_t)(ca + cb + 2) * sizeof(void*));
    r[0] = (void*)(ca + cb);
    r[1] = (void*)(ca + cb);
    for (int64_t i = 0; i < ca; i++) r[i + 2] = arr_a[i + 2];
    for (int64_t i = 0; i < cb; i++) r[ca + i + 2] = arr_b[i + 2];
    return r;
}

// ── Table (string → void* hash map) ──────────────────────────────────────────

typedef struct SlateTableEntry {
    char* key;
    void* val;
    struct SlateTableEntry* next;
} SlateTableEntry;

typedef struct {
    SlateTableEntry** buckets;
    int64_t           count;
    int64_t           cap;
} SlateTable;

static inline void* slate_table_new(void) {
    SlateTable* t = malloc(sizeof(SlateTable));
    t->cap = 16;
    t->count = 0;
    t->buckets = calloc((size_t)t->cap, sizeof(SlateTableEntry*));
    return t;
}

static inline uint64_t _slate_hash_str(const char* s) {
    uint64_t h = 14695981039346656037ULL;
    while (*s) { h ^= (uint8_t)*s++; h *= 1099511628211ULL; }
    return h;
}

static inline void* slate_table_get(void* table, const char* key) {
    if (!table || !key) return NULL;
    SlateTable* t = table;
    uint64_t idx = _slate_hash_str(key) % (uint64_t)t->cap;
    for (SlateTableEntry* e = t->buckets[idx]; e; e = e->next)
        if (strcmp(e->key, key) == 0) return e->val;
    return NULL;
}

static inline int64_t slate_has(void* table, const char* key) {
    if (!table || !key) return 0;
    SlateTable* t = table;
    uint64_t idx = _slate_hash_str(key) % (uint64_t)t->cap;
    for (SlateTableEntry* e = t->buckets[idx]; e; e = e->next)
        if (strcmp(e->key, key) == 0) return 1;
    return 0;
}

static inline void* slate_table_set(void* table, const char* key, void* val) {
    if (!table) table = slate_table_new();
    if (!key) return table;
    SlateTable* t = table;
    uint64_t idx = _slate_hash_str(key) % (uint64_t)t->cap;
    for (SlateTableEntry* e = t->buckets[idx]; e; e = e->next) {
        if (strcmp(e->key, key) == 0) { e->val = val; return table; }
    }
    SlateTableEntry* e = malloc(sizeof(SlateTableEntry));
    e->key  = strdup(key);
    e->val  = val;
    e->next = t->buckets[idx];
    t->buckets[idx] = e;
    t->count++;
    return table;
}

static inline void* slate_keys(void* table) {
    if (!table) return slate_empty_list();
    SlateTable* t = table;
    void** arr = malloc((size_t)(t->count + 2) * sizeof(void*));
    arr[0] = (void*)t->count;
    arr[1] = (void*)t->count;
    int64_t i = 2;
    for (int64_t b = 0; b < t->cap; b++)
        for (SlateTableEntry* e = t->buckets[b]; e; e = e->next)
            arr[i++] = e->key;
    return arr;
}

static inline void* slate_values(void* table) {
    if (!table) return slate_empty_list();
    SlateTable* t = table;
    void** arr = malloc((size_t)(t->count + 2) * sizeof(void*));
    arr[0] = (void*)t->count;
    arr[1] = (void*)t->count;
    int64_t i = 2;
    for (int64_t b = 0; b < t->cap; b++)
        for (SlateTableEntry* e = t->buckets[b]; e; e = e->next)
            arr[i++] = e->val;
    return arr;
}

// ── File IO ───────────────────────────────────────────────────────────────────

static inline char* slate_file_read(const char* path) {
    if (!path) return strdup("");
    FILE* f = fopen(path, "r");
    if (!f) return strdup("");
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return strdup(""); }
    long len = ftell(f);
    if (len < 0) { fclose(f); return strdup(""); }
    fseek(f, 0, SEEK_SET);
    char* buf = malloc((size_t)len + 1);
    if (!buf) { fclose(f); return strdup(""); }
    fread(buf, 1, (size_t)len, f);
    buf[len] = 0;
    fclose(f);
    return buf;
}

static inline int64_t slate_file_write(const char* path, const char* data) {
    if (!path || !data) return 0;
    FILE* f = fopen(path, "w");
    if (!f) return 0;
    fputs(data, f);
    fclose(f);
    return 1;
}

static inline int64_t slate_file_exists(const char* path) {
    if (!path) return 0;
    FILE* f = fopen(path, "r");
    if (!f) return 0;
    fclose(f);
    return 1;
}

static inline int64_t slate_file_append(const char* path, const char* data) {
    if (!path || !data) return 0;
    FILE* f = fopen(path, "a");
    if (!f) return 0;
    fputs(data, f);
    fclose(f);
    return 1;
}

static inline void* slate_file_list(const char* path) {
    if (!path) return slate_empty_list();
    DIR* dir = opendir(path);
    if (!dir) return slate_empty_list();
    void* list = slate_empty_list();
    struct dirent* entry;
    while ((entry = readdir(dir))) {
        if (strcmp(entry->d_name, ".") == 0) continue;
        if (strcmp(entry->d_name, "..") == 0) continue;
        list = slate_add(list, strdup(entry->d_name));
    }
    closedir(dir);
    return list;
}

// ── System ────────────────────────────────────────────────────────────────────

static inline char* slate_home(void) {
    char* h = getenv("HOME");
    return h ? h : (char*)"/tmp";
}

static inline char* slate_env(const char* name) {
    char* v = getenv(name);
    return v ? v : (char*)"";
}

static inline int64_t slate_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static inline int64_t slate_hash(const char* s) {
    if (!s) return 0;
    int64_t h = 0;
    while (*s) { h = h * 31 + (int64_t)(unsigned char)*s++; }
    return h < 0 ? -h : h;
}

// run(cmd, arg1, arg2, ...) — Slate emits slate_run_args(count, cmd, a, b, ...)
static inline char* slate_run_args(int64_t argc, ...) {
    char** argv = malloc((size_t)(argc + 1) * sizeof(char*));
    va_list ap;
    va_start(ap, argc);
    for (int64_t i = 0; i < argc; i++) argv[i] = va_arg(ap, char*);
    va_end(ap);
    argv[argc] = NULL;
    int pipefd[2];
    if (pipe(pipefd) < 0) { free(argv); return strdup(""); }
    pid_t pid = fork();
    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execvp(argv[0], argv);
        _exit(127);
    }
    close(pipefd[1]);
    int status = 0;
    waitpid(pid, &status, 0);
    free(argv);
    size_t cap = 4096;
    size_t len = 0;
    char* out = malloc(cap);
    char buf[4096];
    ssize_t n;
    while ((n = read(pipefd[0], buf, sizeof(buf))) > 0) {
        if (len + (size_t)n >= cap) {
            cap *= 2;
            char* tmp = realloc(out, cap);
            if (!tmp) { free(out); close(pipefd[0]); return strdup(""); }
            out = tmp;
        }
        memcpy(out + len, buf, (size_t)n);
        len += (size_t)n;
    }
    close(pipefd[0]);
    out[len] = 0;
    return out;
}

// run(list) — Slate emits slate_run_list(list) when run() receives a single list argument
static inline char* slate_run_list(void* list) {
    if (!list) return strdup("");
    int64_t count = slate_len(list);
    if (count == 0) return strdup("");
    char** argv = malloc((size_t)(count + 1) * sizeof(char*));
    for (int64_t i = 0; i < count; i++) {
        argv[i] = (char*)slate_get(list, i);
    }
    argv[count] = NULL;
    int pipefd[2];
    if (pipe(pipefd) < 0) { free(argv); return strdup(""); }
    pid_t pid = fork();
    if (pid == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
        execvp(argv[0], argv);
        _exit(127);
    }
    close(pipefd[1]);
    int status = 0;
    waitpid(pid, &status, 0);
    free(argv);
    size_t cap = 4096;
    size_t len = 0;
    char* out = malloc(cap);
    char buf[4096];
    ssize_t n;
    while ((n = read(pipefd[0], buf, sizeof(buf))) > 0) {
        if (len + (size_t)n >= cap) {
            cap *= 2;
            char* tmp = realloc(out, cap);
            if (!tmp) { free(out); close(pipefd[0]); return strdup(""); }
            out = tmp;
        }
        memcpy(out + len, buf, (size_t)n);
        len += (size_t)n;
    }
    close(pipefd[0]);
    out[len] = 0;
    return out;
}

// exec(list) — replaces current process with target binary, streams directly to terminal
static inline void slate_exec(void* list) {
    if (!list) return;
    int64_t count = slate_len(list);
    if (count == 0) return;
    char** argv = malloc((size_t)(count + 1) * sizeof(char*));
    for (int64_t i = 0; i < count; i++) {
        argv[i] = (char*)slate_get(list, i);
    }
    argv[count] = NULL;
    execvp(argv[0], argv);
    perror("exec failed");
    free(argv);
    exit(1);
}

// ── Stdin/Stdout ──────────────────────────────────────────────────────────────

static inline char* slate_read(void) {
    char buf[4096];
    if (!fgets(buf, sizeof(buf), stdin)) return strdup("");
    size_t len = strlen(buf);
    if (len > 0 && buf[len - 1] == '\n') buf[len - 1] = 0;
    return strdup(buf);
}

static inline char* slate_read_n(int64_t n) {
    char* buf = malloc((size_t)(n + 1));
    size_t got = fread(buf, 1, (size_t)n, stdin);
    buf[got] = 0;
    return buf;
}

// ── Args ──────────────────────────────────────────────────────────────────────

static void**  _slate_args     = NULL;
static int64_t _slate_args_len = 0;

static inline void slate_init_args(int argc, char** argv) {
    _slate_args_len = argc > 1 ? argc - 1 : 0;
    _slate_args = malloc((size_t)(_slate_args_len + 2) * sizeof(void*));
    _slate_args[0] = (void*)_slate_args_len;
    _slate_args[1] = (void*)_slate_args_len;
    for (int64_t i = 0; i < _slate_args_len; i++) _slate_args[i + 2] = argv[i + 1];
}

static inline void* slate_args(void) {
    return _slate_args ? _slate_args : slate_empty_list();
}

// ── Option/Result ──────────────────────────────────────────────────────────────

static inline void* slate_str_chars(const char* s) {
    if (!s) return slate_empty_list();
    size_t len = strlen(s);
    void** arr = malloc((len + 2) * sizeof(void*));
    arr[0] = (void*)len;
    arr[1] = (void*)len;
    for (size_t i = 0; i < len; i++) {
        char* ch = malloc(2);
        ch[0] = s[i];
        ch[1] = 0;
        arr[i + 2] = ch;
    }
    return arr;
}

// ── Deprecated bootstrap aliases ─────────────────────────────────────────────
// These exist ONLY so the old self-hosted compiler binary can compile the new
// codegen sources. The old binary emits these function names. Once the compiler
// is rebuilt with the new codegen, delete these — the codegen emits atoll/atof
// and (void*)/(int64_t) casts directly.

static inline int64_t slate_str_to_int(const char* s) {
    return (s && s[0]) ? (int64_t)atoll(s) : 0;
}

static inline double slate_str_to_float(const char* s) {
    return (s && s[0]) ? atof(s) : 0.0;
}

// Old code gen for table(string, int).get/set emits box/unbox.
static inline void* slate_box_int(int64_t n) {
    int64_t* p = malloc(sizeof(int64_t));
    *p = n;
    return p;
}

static inline int64_t slate_unbox_int(void* p) {
    return p ? *(int64_t*)p : 0;
}

#include "slate_parsers.h"

#endif // SLATE_RUNTIME_H
