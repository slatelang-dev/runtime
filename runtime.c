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

int64_t slate_int(char* s) {
    return (int64_t)atoll(s);
}

double slate_float(char* s) {
    return atof(s);
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

char* slate_chars(char* s) {
    int64_t len = strlen(s);
    char* buf = malloc(sizeof(int64_t) + len * sizeof(char*));
    *(int64_t*)buf = len;
    char** items = (char**)(buf + sizeof(int64_t));
    for (int64_t i = 0; i < len; i++) {
        char* ch = malloc(2);
        ch[0] = s[i];
        ch[1] = '\0';
        items[i] = ch;
    }
    return buf;
}

int8_t slate_contains(char* haystack, char* needle) {
    return strstr(haystack, needle) != NULL ? 1 : 0;
}

int8_t slate_starts_with(char* s, char* prefix) {
    return strncmp(s, prefix, strlen(prefix)) == 0 ? 1 : 0;
}

int8_t slate_ends_with(char* s, char* suffix) {
    int sl = strlen(s), fl = strlen(suffix);
    if (fl > sl) return 0;
    return strcmp(s + sl - fl, suffix) == 0 ? 1 : 0;
}

char* slate_replace(char* s, char* from, char* to) {
    char* result = malloc(strlen(s) * 2 + 1);
    char* pos = strstr(s, from);
    if (!pos) { strcpy(result, s); return result; }
    int from_len = strlen(from);
    int to_len = strlen(to);
    int prefix_len = pos - s;
    memcpy(result, s, prefix_len);
    memcpy(result + prefix_len, to, to_len);
    strcpy(result + prefix_len + to_len, pos + from_len);
    return result;
}

char* slate_split(char* s, char* delim) {
    int dlen = strlen(delim);
    int slen = strlen(s);
    char* buf = malloc(sizeof(int64_t) + slen * sizeof(char*));
    *(int64_t*)buf = 0;
    int count = 0;
    char** items = (char**)(buf + sizeof(int64_t));
    char* copy = malloc(slen + 1);
    strcpy(copy, s);
    char* cur = copy;
    while (1) {
        char* found = dlen > 0 ? strstr(cur, delim) : NULL;
        char* part;
        if (!found) {
            part = malloc(strlen(cur) + 1);
            strcpy(part, cur);
            items[count++] = part;
            break;
        }
        int plen = found - cur;
        part = malloc(plen + 1);
        memcpy(part, cur, plen);
        part[plen] = '\0';
        items[count++] = part;
        cur = found + dlen;
        if (dlen == 0) { if (*cur == '\0') break; }
    }
    *(int64_t*)buf = count;
    return buf;
}

void* slate_or(void* option_val, void* default_val) {
    if (option_val == NULL) return default_val;
    return option_val;
}

int8_t slate_exists(void* option_val) {
    return option_val != NULL ? 1 : 0;
}

char* slate_file_path(char* path) {
    return path;
}

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

int8_t slate_file_exists_fn(char* path) {
    FILE* f = fopen(path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}

char* slate_home_fn() {
    char* h = getenv("HOME");
    if (!h) return "";
    return h;
}

char* file_handle(char* path) {
    return path;
}

char* read_file_fn(char* path) {
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

void write_file_fn(char* path, char* content) {
    FILE* f = fopen(path, "w");
    if (!f) return;
    fputs(content, f);
    fclose(f);
}

int8_t exists_file_fn(char* path) {
    FILE* f = fopen(path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}

char* run_shell(char* cmd) {
    char buf[4096];
    snprintf(buf, sizeof(buf), "%s", cmd);
    system(buf);
    return "";
}

char* read_n_fn(int64_t n) {
    char* buf = malloc(n + 1);
    fread(buf, 1, n, stdin);
    buf[n] = '\0';
    return buf;
}

char* read_line_fn() {
    char* buf = malloc(4096);
    if (fgets(buf, 4096, stdin) == NULL) buf[0] = '\0';
    int len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
    return buf;
}

void write_stdout_fn(char* s) {
    fputs(s, stdout);
    fflush(stdout);
}

// ─── Ink color functions (ANSI) ───────────────────────────────────────────────

static char* ansi_wrap(const char* code, char* text) {
    int len = strlen(text);
    int code_len = strlen(code);
    char* buf = malloc(code_len + len + 5);
    sprintf(buf, "%s%s\033[0m", code, text);
    return buf;
}

// Builtin functions matching IR call names
char* file(char* path) { return path; }
char* home() { char* h = getenv("HOME"); return h ? h : ""; }
void run_fn(char* cmd) { system(cmd); }
char* read_n(int64_t n) { char* b = malloc(n+1); fread(b,1,n,stdin); b[n]=0; return b; }
char* read_line() { char* b = malloc(4096); if(!fgets(b,4096,stdin))b[0]=0; int l=strlen(b); if(l>0&&b[l-1]=='\n')b[l-1]=0; return b; }
void write_stdout(char* s) { fputs(s,stdout); fflush(stdout); }