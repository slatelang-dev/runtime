#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// ─── String operations ───────────────────────────────────────────────────────

char* slate_string_concat(char* a, char* b) {
    int la = strlen(a);
    int lb = strlen(b);
    char* buf = malloc(la + lb + 1);
    memcpy(buf, a, la);
    memcpy(buf + la, b, lb);
    buf[la + lb] = '\0';
    return buf;
}

char* slate_string(int64_t n) {
    char* buf = malloc(32);
    snprintf(buf, 32, "%lld", (long long)n);
    return buf;
}

// ─── Print ───────────────────────────────────────────────────────────────────

void slate_print(char* s) {
    puts(s);
}

// ─── List (ptr array with length prefix) ─────────────────────────────────────
// Layout: [int64 length][ptr items...]

char* slate_list_new() {
    char* buf = malloc(sizeof(int64_t));
    *(int64_t*)buf = 0;
    return buf;
}

int64_t slate_len(char* p) {
    return *(int64_t*)p;
}

char* slate_get(char* p, int64_t index) {
    int64_t len = *(int64_t*)p;
    if (index < 0 || index >= len) {
        fprintf(stderr, "panic: index out of bounds\n");
        exit(1);
    }
    char** items = (char**)(p + sizeof(int64_t));
    return items[index];
}

char* slate_add(char* p, char* val) {
    int64_t len = *(int64_t*)p;
    int64_t new_len = len + 1;
    char* buf = malloc(sizeof(int64_t) + new_len * sizeof(char*));
    *(int64_t*)buf = new_len;
    char** items = (char**)(buf + sizeof(int64_t));
    char** old_items = (char**)(p + sizeof(int64_t));
    for (int64_t i = 0; i < len; i++) {
        items[i] = old_items[i];
    }
    items[len] = val;
    return buf;
}

// ─── Table (parallel key/value arrays with length prefix) ────────────────────
// Layout: [int64 length][ptr keys...][ptr values...]

char* slate_table_new() {
    char* buf = malloc(sizeof(int64_t));
    *(int64_t*)buf = 0;
    return buf;
}

int64_t slate_table_len(char* p) {
    return *(int64_t*)p;
}

int8_t slate_has(char* p, char* key) {
    int64_t len = *(int64_t*)p;
    char** keys = (char**)(p + sizeof(int64_t));
    for (int64_t i = 0; i < len; i++) {
        if (strcmp(keys[i], key) == 0) return 1;
    }
    return 0;
}

char* slate_get_table(char* p, char* key) {
    int64_t len = *(int64_t*)p;
    char** keys = (char**)(p + sizeof(int64_t));
    char** vals = (char**)(p + sizeof(int64_t) + len * sizeof(char*));
    for (int64_t i = 0; i < len; i++) {
        if (strcmp(keys[i], key) == 0) return vals[i];
    }
    return "";
}

char* slate_set(char* p, char* key, char* val) {
    int64_t len = *(int64_t*)p;
    // Check if key exists — update in place
    char** keys = (char**)(p + sizeof(int64_t));
    char** vals = (char**)(p + sizeof(int64_t) + len * sizeof(char*));
    for (int64_t i = 0; i < len; i++) {
        if (strcmp(keys[i], key) == 0) {
            int64_t new_size = sizeof(int64_t) + len * sizeof(char*) * 2;
            char* buf = malloc(new_size);
            memcpy(buf, p, new_size);
            char** new_vals = (char**)(buf + sizeof(int64_t) + len * sizeof(char*));
            new_vals[i] = val;
            return buf;
        }
    }
    // New key
    int64_t new_len = len + 1;
    char* buf = malloc(sizeof(int64_t) + new_len * sizeof(char*) * 2);
    *(int64_t*)buf = new_len;
    char** new_keys = (char**)(buf + sizeof(int64_t));
    char** new_vals = (char**)(buf + sizeof(int64_t) + new_len * sizeof(char*));
    for (int64_t i = 0; i < len; i++) {
        new_keys[i] = keys[i];
        new_vals[i] = vals[i];
    }
    new_keys[len] = key;
    new_vals[len] = val;
    return buf;
}

// ─── Input / args ─────────────────────────────────────────────────────────────

char* slate_input() {
    char* buf = malloc(4096);
    if (fgets(buf, 4096, stdin) == NULL) {
        buf[0] = '\0';
    }
    // Strip trailing newline
    int len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
    return buf;
}

// args handle — stored as a list
static char** _slate_argv = NULL;
static int64_t _slate_argc = 0;

void slate_init_args(int argc, char** argv) {
    _slate_argc = argc;
    _slate_argv = argv;
}

char* slate_args(char* handle) {
    char* buf = malloc(sizeof(int64_t) + _slate_argc * sizeof(char*));
    *(int64_t*)buf = _slate_argc;
    char** items = (char**)(buf + sizeof(int64_t));
    for (int64_t i = 0; i < _slate_argc; i++) {
        items[i] = _slate_argv[i];
    }
    return buf;
}

// ─── Ink color functions (ANSI) ───────────────────────────────────────────────

static char* ansi_wrap(const char* code, char* text) {
    int len = strlen(text);
    int code_len = strlen(code);
    char* buf = malloc(code_len + len + 5);
    sprintf(buf, "%s%s\033[0m", code, text);
    return buf;
}

char* bold(char* s)        { return ansi_wrap("\033[1m", s); }
char* dim(char* s)         { return ansi_wrap("\033[2m", s); }
char* italic(char* s)      { return ansi_wrap("\033[3m", s); }
char* underline(char* s)   { return ansi_wrap("\033[4m", s); }
char* red(char* s)         { return ansi_wrap("\033[38;2;220;50;50m", s); }
char* green(char* s)       { return ansi_wrap("\033[38;2;50;200;80m", s); }
char* blue(char* s)        { return ansi_wrap("\033[38;2;50;120;220m", s); }
char* yellow(char* s)      { return ansi_wrap("\033[38;2;230;200;50m", s); }
char* orange(char* s)      { return ansi_wrap("\033[38;2;230;120;30m", s); }
char* purple(char* s)      { return ansi_wrap("\033[38;2;150;80;220m", s); }
char* slate_color(char* s) { return ansi_wrap("\033[38;2;112;128;144m", s); }
char* gray(char* s)        { return ansi_wrap("\033[38;2;140;140;140m", s); }
char* white(char* s)       { return ansi_wrap("\033[38;2;240;240;240m", s); }
char* bold_red(char* s)    { return bold(red(s)); }
char* bold_green(char* s)  { return bold(green(s)); }
char* bold_yellow(char* s) { return bold(yellow(s)); }
char* bold_blue(char* s)   { return bold(blue(s)); }
char* bold_slate(char* s)  { return bold(slate_color(s)); }