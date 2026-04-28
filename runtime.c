#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

// ─── String operations ────────────────────────────────────────────────────────

char* slate_string_concat(char* a, char* b) {
    int la = strlen(a), lb = strlen(b);
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

int64_t slate_int(char* s) { return (int64_t)atoll(s); }
double slate_float(char* s) { return atof(s); }

// ─── Print ────────────────────────────────────────────────────────────────────

void slate_print(char* s) { puts(s); }

// ─── List ─────────────────────────────────────────────────────────────────────
// Layout: [int64 length][ptr items...]

char* slate_list_new() {
    char* buf = malloc(sizeof(int64_t));
    *(int64_t*)buf = 0;
    return buf;
}

int64_t slate_len(char* p) { return *(int64_t*)p; }

char* slate_get(char* p, int64_t index) {
    int64_t len = *(int64_t*)p;
    if (index < 0 || index >= len) {
        fprintf(stderr, "panic: index out of bounds (%lld of %lld)\n", (long long)index, (long long)len);
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
    char** old = (char**)(p + sizeof(int64_t));
    for (int64_t i = 0; i < len; i++) items[i] = old[i];
    items[len] = val;
    return buf;
}

// ─── Table ────────────────────────────────────────────────────────────────────
// Layout: [int64 length][ptr keys...][ptr values...]

char* slate_table_new() {
    char* buf = malloc(sizeof(int64_t));
    *(int64_t*)buf = 0;
    return buf;
}

int64_t slate_table_len(char* p) { return *(int64_t*)p; }

int8_t slate_has(char* p, char* key) {
    int64_t len = *(int64_t*)p;
    char** keys = (char**)(p + sizeof(int64_t));
    for (int64_t i = 0; i < len; i++)
        if (strcmp(keys[i], key) == 0) return 1;
    return 0;
}

char* slate_get_table(char* p, char* key) {
    int64_t len = *(int64_t*)p;
    char** keys = (char**)(p + sizeof(int64_t));
    char** vals = (char**)(p + sizeof(int64_t) + len * sizeof(char*));
    for (int64_t i = 0; i < len; i++)
        if (strcmp(keys[i], key) == 0) return vals[i];
    return "";
}

char* slate_set(char* p, char* key, char* val) {
    int64_t len = *(int64_t*)p;
    char** keys = (char**)(p + sizeof(int64_t));
    char** vals = (char**)(p + sizeof(int64_t) + len * sizeof(char*));
    // Update existing key
    for (int64_t i = 0; i < len; i++) {
        if (strcmp(keys[i], key) == 0) {
            int64_t sz = sizeof(int64_t) + len * sizeof(char*) * 2;
            char* buf = malloc(sz);
            memcpy(buf, p, sz);
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
    for (int64_t i = 0; i < len; i++) { new_keys[i] = keys[i]; new_vals[i] = vals[i]; }
    new_keys[len] = key;
    new_vals[len] = val;
    return buf;
}

// ─── Args ─────────────────────────────────────────────────────────────────────

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
    for (int64_t i = 0; i < _slate_argc; i++) items[i] = _slate_argv[i];
    return buf;
}

// ─── String methods ───────────────────────────────────────────────────────────

char* slate_chars(char* s) {
    int64_t len = strlen(s);
    char* buf = malloc(sizeof(int64_t) + len * sizeof(char*));
    *(int64_t*)buf = len;
    char** items = (char**)(buf + sizeof(int64_t));
    for (int64_t i = 0; i < len; i++) {
        char* ch = malloc(2);
        ch[0] = s[i]; ch[1] = '\0';
        items[i] = ch;
    }
    return buf;
}

int8_t slate_contains(char* h, char* n) { return strstr(h, n) != NULL ? 1 : 0; }
int8_t slate_starts_with(char* s, char* p) { return strncmp(s, p, strlen(p)) == 0 ? 1 : 0; }

int8_t slate_ends_with(char* s, char* suf) {
    int sl = strlen(s), fl = strlen(suf);
    if (fl > sl) return 0;
    return strcmp(s + sl - fl, suf) == 0 ? 1 : 0;
}

char* slate_replace(char* s, char* from, char* to) {
    char* pos = strstr(s, from);
    if (!pos) { char* r = malloc(strlen(s) + 1); strcpy(r, s); return r; }
    int fl = strlen(from), tl = strlen(to), pl = pos - s;
    char* result = malloc(strlen(s) + tl + 1);
    memcpy(result, s, pl);
    memcpy(result + pl, to, tl);
    strcpy(result + pl + tl, pos + fl);
    return result;
}

char* slate_split(char* s, char* delim) {
    int dlen = strlen(delim), slen = strlen(s);
    // Allocate conservatively
    char* buf = malloc(sizeof(int64_t) + (slen + 1) * sizeof(char*));
    int count = 0;
    char** items = (char**)(buf + sizeof(int64_t));
    char* copy = malloc(slen + 1);
    strcpy(copy, s);
    char* cur = copy;
    while (1) {
        char* found = (dlen > 0) ? strstr(cur, delim) : NULL;
        if (!found) {
            char* part = malloc(strlen(cur) + 1);
            strcpy(part, cur);
            items[count++] = part;
            break;
        }
        int plen = found - cur;
        char* part = malloc(plen + 1);
        memcpy(part, cur, plen);
        part[plen] = '\0';
        items[count++] = part;
        cur = found + dlen;
    }
    *(int64_t*)buf = count;
    return buf;
}

// ─── Option ───────────────────────────────────────────────────────────────────

void* slate_or(void* v, void* def) { return v == NULL ? def : v; }
int8_t slate_exists(void* v) { return v != NULL ? 1 : 0; }

// ─── File operations ──────────────────────────────────────────────────────────
// file(path) returns the path as a handle — methods operate on it

char* file(char* path) { return path; }
char* home() { char* h = getenv("HOME"); return h ? h : ""; }

char* slate_file_read(char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return "";
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);
    char* buf = malloc(len + 1);
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

void slate_file_write(char* path, char* content) {
    FILE* f = fopen(path, "w");
    if (!f) return;
    fputs(content, f);
    fclose(f);
}

int8_t slate_file_exists(char* path) {
    FILE* f = fopen(path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}

char* slate_file_path(char* path) { return path; }

// ─── IO ───────────────────────────────────────────────────────────────────────

// read() — read a line from stdin
char* read() {
    char* buf = malloc(4096);
    if (!fgets(buf, 4096, stdin)) buf[0] = '\0';
    int l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
    return buf;
}

// read_n(n) — read n bytes from stdin
char* read_n(int64_t n) {
    char* buf = malloc(n + 1);
    int64_t got = fread(buf, 1, n, stdin);
    buf[got] = '\0';
    return buf;
}

// write(s) — write to stdout without newline (used by LSP)
void slate_write(char* s) {
    fputs(s, stdout);
    fflush(stdout);
}

// ─── run(cmd, args...) ────────────────────────────────────────────────────────
// Varargs — build command string from all non-null args

void run(char* cmd, ...) {
    char buf[8192];
    int pos = 0;
    if (cmd) pos += snprintf(buf + pos, sizeof(buf) - pos, "%s", cmd);
    va_list args;
    va_start(args, cmd);
    char* arg;
    while ((arg = va_arg(args, char*)) != NULL) {
        if (arg[0] != '\0')
            pos += snprintf(buf + pos, sizeof(buf) - pos, " %s", arg);
    }
    va_end(args);
    system(buf);
}

char* slate_input() {
    char* buf = malloc(4096);
    if (!fgets(buf, 4096, stdin)) buf[0] = '\0';
    int l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
    return buf;
}
