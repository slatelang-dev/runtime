#ifndef SLATE_RUNTIME_H
#define SLATE_RUNTIME_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdarg.h>
#include <sys/wait.h>

// ── Structs ───────────────────────────────────────────────────────────────────

typedef struct {
    int64_t success;
    char*   message;
    void*   node;
    char*   source;
} slate_ParseResult;

typedef struct {
    int64_t success;
    void*   errors;
    void*   warnings;
    char*   source;
} slate_AnalysisResult;

typedef struct {
    char*   name;
    char*   path;
    char*   source;
    void*   ast;
    int8_t  success;
    char*   error;
} slate_LoadedModule;

typedef struct {
    char*   name;
    char*   version;
    char*   description;
    char*   entry;
    char*   bin;
    int8_t  bare;
} slate_Project;

typedef struct {
    char* name;
    char* path;
} slate_ModuleInfo;

// ── Memory ────────────────────────────────────────────────────────────────────

static inline void* slate_alloc(int64_t size) {
    return malloc((size_t)size);
}

static inline void slate_free(void* ptr) {
    free(ptr);
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

static inline int64_t slate_str_to_int(const char* s) {
    return (s && s[0]) ? (int64_t)atoll(s) : 0;
}

static inline double slate_str_to_float(const char* s) {
    return (s && s[0]) ? atof(s) : 0.0;
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
    // Returns a void** tagged list: [int64_t count, char* item0, char* item1, ...]
    if (!s || !delim) {
        void** r = malloc(2 * sizeof(void*));
        r[0] = (void*)(int64_t)0;
        return r;
    }
    size_t dlen = strlen(delim);
    // Count parts
    size_t count = 1;
    const char* p = s;
    while ((p = strstr(p, delim))) { count++; p += dlen; }
    void** arr = malloc((count + 1) * sizeof(void*));
    arr[0] = (void*)count;
    p = s;
    size_t idx = 1;
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
// Tagged void** layout: arr[0] = (void*)count, arr[1..count] = items

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
    return arr[idx + 1];
}

static inline void* slate_add(void* list, void* val) {
    int64_t count = list ? (int64_t)((void**)list)[0] : 0;
    void** arr = malloc((size_t)(count + 2) * sizeof(void*));
    arr[0] = (void*)(count + 1);
    if (list) memcpy(arr + 1, (void**)list + 1, (size_t)count * sizeof(void*));
    arr[count + 1] = val;
    return arr;
}

static inline void* slate_empty_list(void) {
    void** arr = malloc(sizeof(void*));
    arr[0] = (void*)(int64_t)0;
    return arr;
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
    if (!table || !key) return table;
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
    void** arr = malloc((size_t)(t->count + 1) * sizeof(void*));
    arr[0] = (void*)t->count;
    int64_t i = 1;
    for (int64_t b = 0; b < t->cap; b++)
        for (SlateTableEntry* e = t->buckets[b]; e; e = e->next)
            arr[i++] = e->key;
    return arr;
}

static inline void* slate_values(void* table) {
    if (!table) return slate_empty_list();
    SlateTable* t = table;
    void** arr = malloc((size_t)(t->count + 1) * sizeof(void*));
    arr[0] = (void*)t->count;
    int64_t i = 1;
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
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = malloc((size_t)len + 1);
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

static inline const char* slate_file(const char* path) {
    return path;
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
static inline int64_t slate_run_args(int64_t argc, ...) {
    // Build argv from varargs
    char** argv = malloc((size_t)(argc + 1) * sizeof(char*));
    va_list ap;
    va_start(ap, argc);
    for (int64_t i = 0; i < argc; i++) argv[i] = va_arg(ap, char*);
    va_end(ap);
    argv[argc] = NULL;
    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[0], argv);
        _exit(127);
    }
    int status = 0;
    waitpid(pid, &status, 0);
    free(argv);
    return WEXITSTATUS(status);
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

static inline void slate_write(const char* s) {
    if (s) { fputs(s, stdout); fflush(stdout); }
}

// ── Args ──────────────────────────────────────────────────────────────────────

static void**  _slate_args     = NULL;
static int64_t _slate_args_len = 0;

static inline void slate_init_args(int argc, char** argv) {
    _slate_args_len = argc > 1 ? argc - 1 : 0;
    _slate_args = malloc((size_t)(_slate_args_len + 1) * sizeof(void*));
    _slate_args[0] = (void*)_slate_args_len;
    for (int64_t i = 0; i < _slate_args_len; i++) _slate_args[i + 1] = argv[i + 1];
}

static inline void* slate_args(void) {
    return _slate_args ? _slate_args : slate_empty_list();
}

// ── Colors ────────────────────────────────────────────────────────────────────

static inline char* red(const char* s)         { return slate_concat(slate_concat("\033[38;2;220;50;50m",  s), "\033[0m"); }
static inline char* green(const char* s)       { return slate_concat(slate_concat("\033[38;2;50;200;80m",  s), "\033[0m"); }
static inline char* blue(const char* s)        { return slate_concat(slate_concat("\033[38;2;50;120;220m", s), "\033[0m"); }
static inline char* yellow(const char* s)      { return slate_concat(slate_concat("\033[38;2;230;200;50m", s), "\033[0m"); }
static inline char* orange(const char* s)      { return slate_concat(slate_concat("\033[38;2;230;120;30m", s), "\033[0m"); }
static inline char* purple(const char* s)      { return slate_concat(slate_concat("\033[38;2;150;80;220m", s), "\033[0m"); }
static inline char* gray(const char* s)        { return slate_concat(slate_concat("\033[38;2;140;140;140m",s), "\033[0m"); }
static inline char* white(const char* s)       { return slate_concat(slate_concat("\033[38;2;240;240;240m",s), "\033[0m"); }
static inline char* slate_color(const char* s) { return slate_concat(slate_concat("\033[38;2;112;128;144m",s), "\033[0m"); }
static inline char* bold(const char* s)        { return slate_concat(slate_concat("\033[1m", s), "\033[0m"); }
static inline char* dim(const char* s)         { return slate_concat(slate_concat("\033[2m", s), "\033[0m"); }
static inline char* italic(const char* s)      { return slate_concat(slate_concat("\033[3m", s), "\033[0m"); }
static inline char* underline(const char* s)   { return slate_concat(slate_concat("\033[4m", s), "\033[0m"); }
static inline char* bold_slate(const char* s)  { return bold(slate_color(s)); }
static inline char* bold_green(const char* s)  { return bold(green(s)); }
static inline char* bold_yellow(const char* s) { return bold(yellow(s)); }
static inline char* bold_blue(const char* s)   { return bold(blue(s)); }
static inline char* bold_red(const char* s)    { return bold(red(s)); }

// ── Forward declarations ──────────────────────────────────────────────────────
// These are implemented by the compiled Slate core .o files, not here.

void* slate_parse(const char* source);
void* slate_analyse(void* ast, const char* path);
void* slate_analyse_bare(void* ast, const char* path, int8_t bare);
char* slate_render_all(void* errors, void* warnings, const char* source);
void* load_project_file(void);
int64_t project_valid(void* project);
void* load_entry(void* project);
void* discover_modules(void* ast, char* source_dir, void* seen);
void* load_modules(void* stubs);
void* build_registry_from_modules(void* modules);
char* walk_module(void* ast, void* reg);

#endif // SLATE_RUNTIME_H
