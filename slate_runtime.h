#ifndef SLATE_RUNTIME_H
#define SLATE_RUNTIME_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

typedef struct {
    int64_t success;
    char* message;
    void* node;
    char* source;
} slate_ParseResult;

typedef struct {
    int64_t success;
    void* errors;
    void* warnings;
    char* source;
} slate_AnalysisResult;

typedef struct {
    char* name;
    char* path;
    char* source;
    void* ast;
    int64_t loaded;
    char* load_error;
} slate_LoadedModule;

typedef struct {
    char* name;
    char* entry;
    int64_t bare;
    char* bin;
} slate_Project;

typedef struct {
    char* name;
    char* path;
} slate_ModuleInfo;

static inline void* ModuleInfo(char* path, char* name) {
    slate_ModuleInfo* mi = malloc(sizeof(slate_ModuleInfo));
    mi->path = path;
    mi->name = name;
    return mi;
}

static inline void* parse(char* source) { return NULL; }
static inline void* analyse(void* ast, char* path, int64_t bare) { return NULL; }
static inline void* analyse_bare(void* ast, char* path, int64_t bare) { return NULL; }
static inline char* render_all(void* errors, void* warnings, char* source) { return ""; }
static inline void* load_module(char* name, char* path) { return NULL; }
static inline void* load_entry(void* project) { return NULL; }
static inline void* load_project_file() { return NULL; }
static inline int64_t project_valid(void* project) { return 0; }
static inline void* collect_module_asts(void* ast, char* source_dir, void* list) { return list; }
static inline void* walk_module(void* ast, void* reg) { return NULL; }
static inline void* build_registry_from_modules(void* modules) { return NULL; }

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
    return (void*)arr;
}

static inline int64_t slate_len(void* list) {
    return 0;
}

static inline void* slate_get(void* list, int64_t idx) {
    void** arr = (void**)list;
    return arr[idx];
}

static inline void slate_set(void* list, int64_t idx, void* val) {
    void** arr = (void**)list;
    arr[idx] = val;
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

static inline char* slate_concat(char* a, char* b) {
    if (a == NULL) a = "";
    if (b == NULL) b = "";
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    char* result = malloc(len_a + len_b + 1);
    memcpy(result, a, len_a);
    memcpy(result + len_a, b, len_b);
    result[len_a + len_b] = 0;
    return result;
}

static inline char* slate_ansi_reset() { return "\033[0m"; }
static inline char* slate_ansi_bold() { return "\033[1m"; }
static inline char* slate_ansi_dim() { return "\033[2m"; }
static inline char* slate_ansi_italic() { return "\033[3m"; }
static inline char* slate_ansi_underline() { return "\033[4m"; }

static inline char* slate_red(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat("\033[38;2;220;50;50m", s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_green(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat("\033[38;2;50;200;80m", s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_blue(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat("\033[38;2;50;120;220m", s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_yellow(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat("\033[38;2;230;200;50m", s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_orange(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat("\033[38;2;230;120;30m", s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_purple(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat("\033[38;2;150;80;220m", s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_slate_color(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat("\033[38;2;112;128;144m", s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_gray(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat("\033[38;2;140;140;140m", s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_white(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat("\033[38;2;240;240;240m", s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_black(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat("\033[38;2;20;20;20m", s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_bold(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat(slate_ansi_bold(), s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_dim(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat(slate_ansi_dim(), s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_italic(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat(slate_ansi_italic(), s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_underline(char* s) {
    if (s == NULL) s = "";
    char* r = slate_concat(slate_ansi_underline(), s);
    return slate_concat(r, slate_ansi_reset());
}
static inline char* slate_bold_slate(char* s) { return slate_bold(slate_slate_color(s)); }
static inline char* slate_bold_green(char* s) { return slate_bold(slate_green(s)); }
static inline char* slate_bold_yellow(char* s) { return slate_bold(slate_yellow(s)); }
static inline char* slate_bold_blue(char* s) { return slate_bold(slate_blue(s)); }

static inline char* red(char* s) { return slate_red(s); }
static inline char* green(char* s) { return slate_green(s); }
static inline char* blue(char* s) { return slate_blue(s); }
static inline char* yellow(char* s) { return slate_yellow(s); }
static inline char* orange(char* s) { return slate_orange(s); }
static inline char* purple(char* s) { return slate_purple(s); }
static inline char* slate_color(char* s) { return slate_slate_color(s); }
static inline char* gray(char* s) { return slate_gray(s); }
static inline char* white(char* s) { return slate_white(s); }
static inline char* black(char* s) { return slate_black(s); }
static inline char* bold(char* s) { return slate_bold(s); }
static inline char* dim(char* s) { return slate_dim(s); }
static inline char* italic(char* s) { return slate_italic(s); }
static inline char* underline(char* s) { return slate_underline(s); }
static inline char* bold_slate(char* s) { return slate_bold_slate(s); }
static inline char* bold_green(char* s) { return slate_bold_green(s); }
static inline char* bold_yellow(char* s) { return slate_bold_yellow(s); }
static inline char* bold_blue(char* s) { return slate_bold_blue(s); }

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
    char* start = s;
    while (*start == ' ' || *start == '\t' || *start == '\n' || *start == '\r') start++;
    char* end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) end--;
    size_t len = (size_t)(end - start + 1);
    char* result = malloc(len + 1);
    strncpy(result, start, len);
    result[len] = 0;
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
        h = h * 31 + (int64_t)(unsigned char)*s;
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

static inline void* slate_empty() {
    return NULL;
}

static inline void* slate_table() {
    return slate_list_make(16);
}

static inline void* slate_list(int64_t cap) {
    return slate_list_make(cap);
}

static inline void* table_get(void* table, void* key) {
    return NULL;
}

static inline void table_set(void* table, void* key, void* val) {
}

static inline int64_t table_has(void* table, void* key) {
    return 0;
}

static inline void* table_keys(void* table) {
    return NULL;
}

static inline void* table_values(void* table) {
    return NULL;
}

static inline int64_t slate_run_varargs(int64_t argc, void** argv) {
    return 0;
}

static inline void* slate_table_set(void* table, void* key, void* val) {
    return table;
}

static inline char* slate_read() {
    char buf[4096];
    if (fgets(buf, sizeof(buf), stdin) == NULL) return "";
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = 0;
    return strdup(buf);
}

static inline char* slate_read_n(int64_t n) {
    char* buf = malloc((size_t)(n + 1));
    size_t read = fread(buf, 1, (size_t)n, stdin);
    buf[read] = 0;
    return buf;
}

static inline void slate_write(char* s) {
    if (s) fputs(s, stdout);
    fflush(stdout);
}

static void** _slate_args = NULL;
static int64_t _slate_args_len = 0;

static inline void slate_init_args(int argc, char** argv) {
    _slate_args_len = argc > 1 ? argc - 1 : 0;
    _slate_args = malloc(sizeof(void*) * (size_t)(_slate_args_len + 1));
    for (int64_t i = 0; i < _slate_args_len; i++) {
        _slate_args[i] = argv[i + 1];
    }
}

static inline void* slate_args() {
    return (void*)_slate_args;
}

void* slate_parse(char* source);
void* slate_analyse(void* ast, char* path);
void* slate_analyse_bare(void* ast, char* path, int8_t bare);
char* slate_render_all(void* errors, void* warnings, char* source);
void* slate_tokenize(char* source);
void* slate_json(char* source);
void* slate_toml(char* source);

int main(int argc, char** argv) {
    slate_init_args(argc, argv);
    main();
    return 0;
}

#endif
