#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h>

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

void slate_print(char* s) { puts(s); }

char* slate_list_new() {
    char* buf = malloc(sizeof(int64_t) * 2);
    *(int64_t*)buf = 0;
    *((int64_t*)buf + 1) = 0;
    return buf;
}

int64_t slate_len(char* p) { return *(int64_t*)p; }

char* slate_join_lines(char* list_ptr) {
    int64_t len = *(int64_t*)list_ptr;
    if (len == 0) {
        char* r = malloc(1);
        r[0] = '\0';
        return r;
    }
    char** items = (char**)(list_ptr + sizeof(int64_t) * 2);
    int64_t total = 0;
    for (int64_t i = 0; i < len; i++) {
        total += strlen(items[i]);
        total += 1;
    }
    char* result = malloc(total);
    int64_t pos = 0;
    for (int64_t i = 0; i < len; i++) {
        char* item = items[i];
        int item_len = strlen(item);
        memcpy(result + pos, item, item_len);
        pos += item_len;
        result[pos++] = '\n';
    }
    result[total - 1] = '\0';
    return result;
}

char* slate_escape_string(char* s) {
    int len = strlen(s);
    int64_t header_size = sizeof(int64_t) * 2;
    int64_t cap = len * 2 + 1;
    char* buf = malloc(header_size + cap * sizeof(char*));
    *(int64_t*)buf = len;
    *((int64_t*)buf + 1) = cap;
    char** items = (char**)(buf + header_size);
    int64_t pos = 0;
    for (int i = 0; i < len; i++) {
        char c = s[i];
        if (c == '\n') {
            char* esc = malloc(4);
            esc[0] = '\\'; esc[1] = '0'; esc[2] = 'A'; esc[3] = '\0';
            items[pos++] = esc;
        } else if (c == '\t') {
            char* esc = malloc(4);
            esc[0] = '\\'; esc[1] = '0'; esc[2] = '9'; esc[3] = '\0';
            items[pos++] = esc;
        } else if (c == '\r') {
            char* esc = malloc(4);
            esc[0] = '\\'; esc[1] = '0'; esc[2] = 'D'; esc[3] = '\0';
            items[pos++] = esc;
        } else if (c == '\\') {
            char* esc = malloc(3);
            esc[0] = '\\'; esc[1] = '\\'; esc[2] = '\0';
            items[pos++] = esc;
        } else if (c == '"') {
            char* esc = malloc(4);
            esc[0] = '\\'; esc[1] = '2'; esc[2] = '2'; esc[3] = '\0';
            items[pos++] = esc;
        } else {
            char* ch = malloc(2);
            ch[0] = c; ch[1] = '\0';
            items[pos++] = ch;
        }
    }
    *(int64_t*)buf = pos;
    return buf;
}

char* slate_join_chars(char* list_ptr) {
    int64_t len = *(int64_t*)list_ptr;
    char** items = (char**)(list_ptr + sizeof(int64_t) * 2);
    int64_t total = 0;
    for (int64_t i = 0; i < len; i++) total += strlen(items[i]);
    char* result = malloc(total + 1);
    int64_t pos = 0;
    for (int64_t i = 0; i < len; i++) {
        char* item = items[i];
        int item_len = strlen(item);
        memcpy(result + pos, item, item_len);
        pos += item_len;
    }
    result[total] = '\0';
    return result;
}

char* slate_escape_ll(char* s) {
    int len = strlen(s);
    char* result = malloc(len * 4 + 1);
    int pos = 0;
    for (int i = 0; i < len; i++) {
        char c = s[i];
        if (c == '\\') { result[pos++] = '\\'; result[pos++] = '\\'; }
        else if (c == '"') { result[pos++] = '\\'; result[pos++] = '2'; result[pos++] = '2'; }
        else if (c == '\n') { result[pos++] = '\\'; result[pos++] = '0'; result[pos++] = 'A'; }
        else if (c == '\t') { result[pos++] = '\\'; result[pos++] = '0'; result[pos++] = '9'; }
        else if (c == '\r') { result[pos++] = '\\'; result[pos++] = '0'; result[pos++] = 'D'; }
        else { result[pos++] = c; }
    }
    result[pos] = '\0';
    return result;
}

int64_t slate_strlen(char* s) { return strlen(s); }

char* slate_concat3(char* a, char* b, char* c) {
    int la = strlen(a), lb = strlen(b), lc = strlen(c);
    char* buf = malloc(la + lb + lc + 1);
    memcpy(buf, a, la);
    memcpy(buf + la, b, lb);
    memcpy(buf + la + lb, c, lc);
    buf[la + lb + lc] = '\0';
    return buf;
}

char* slate_concat4(char* a, char* b, char* c, char* d) {
    int la = strlen(a), lb = strlen(b), lc = strlen(c), ld = strlen(d);
    char* buf = malloc(la + lb + lc + ld + 1);
    memcpy(buf, a, la);
    memcpy(buf + la, b, lb);
    memcpy(buf + la + lb, c, lc);
    memcpy(buf + la + lb + lc, d, ld);
    buf[la + lb + lc + ld] = '\0';
    return buf;
}

char* slate_concat5(char* a, char* b, char* c, char* d, char* e) {
    int la = strlen(a), lb = strlen(b), lc = strlen(c), ld = strlen(d), le = strlen(e);
    char* buf = malloc(la + lb + lc + ld + le + 1);
    memcpy(buf, a, la);
    memcpy(buf + la, b, lb);
    memcpy(buf + la + lb, c, lc);
    memcpy(buf + la + lb + lc, d, ld);
    memcpy(buf + la + lb + lc + ld, e, le);
    buf[la + lb + lc + ld + le] = '\0';
    return buf;
}

char* slate_get(char* p, int64_t index) {
    int64_t len = *(int64_t*)p;
    if (index < 0 || index >= len) {
        fprintf(stderr, "panic: index out of bounds (%lld of %lld)\n", (long long)index, (long long)len);
        exit(1);
    }
    char** items = (char**)(p + sizeof(int64_t) * 2);
    return items[index];
}

char* slate_add(char* p, char* val) {
    int64_t len = *(int64_t*)p;
    int64_t cap = *((int64_t*)p + 1);
    
    if (len < cap) {
        char** items = (char**)(p + sizeof(int64_t) * 2);
        items[len] = val;
        *(int64_t*)p = len + 1;
        return p;
    }
    
    int64_t new_cap = cap == 0 ? 8 : cap * 2;
    int64_t new_len = len + 1;
    int64_t header_size = sizeof(int64_t) * 2;
    int64_t buf_size = header_size + new_cap * sizeof(char*);
    char* buf = malloc(buf_size);
    *(int64_t*)buf = new_len;
    *((int64_t*)buf + 1) = new_cap;
    char** items = (char**)(buf + header_size);
    char** old = (char**)(p + header_size);
    for (int64_t i = 0; i < len; i++) items[i] = old[i];
    items[len] = val;
    free(p);
    return buf;
}

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
    for (int64_t i = 0; i < len; i++) {
        if (strcmp(keys[i], key) == 0) {
            int64_t sz = sizeof(int64_t) + len * sizeof(char*) * 2;
            char* buf = malloc(sz);
            memcpy(buf, p, sz);
            char** new_vals = (char**)(buf + sizeof(int64_t) + len * sizeof(char*));
            new_vals[i] = val;
            free(p);
            return buf;
        }
    }
    int64_t new_len = len + 1;
    char* buf = malloc(sizeof(int64_t) + new_len * sizeof(char*) * 2);
    *(int64_t*)buf = new_len;
    char** new_keys = (char**)(buf + sizeof(int64_t));
    char** new_vals = (char**)(buf + sizeof(int64_t) + new_len * sizeof(char*));
    for (int64_t i = 0; i < len; i++) { new_keys[i] = keys[i]; new_vals[i] = vals[i]; }
    new_keys[len] = key;
    new_vals[len] = val;
    free(p);
    return buf;
}

static char** _slate_argv = NULL;
static int64_t _slate_argc = 0;

void slate_init_args(int argc, char** argv) {
    _slate_argc = argc;
    _slate_argv = argv;
}

char* slate_args(char* handle) {
    int64_t header_size = sizeof(int64_t) * 2;
    int64_t cap = _slate_argc > 0 ? _slate_argc : 8;
    char* buf = malloc(header_size + cap * sizeof(char*));
    *(int64_t*)buf = _slate_argc;
    *((int64_t*)buf + 1) = cap;
    char** items = (char**)(buf + header_size);
    for (int64_t i = 0; i < _slate_argc; i++) items[i] = _slate_argv[i];
    return buf;
}

static char* _cached_source = NULL;
static char* _cached_chars = NULL;
static int64_t _cached_len = 0;

char* slate_chars(char* s) {
    int64_t len = strlen(s);
    if (_cached_source == s && _cached_len == len && _cached_chars != NULL) {
        return _cached_chars;
    }
    int64_t header_size = sizeof(int64_t) * 2;
    int64_t buflen = header_size + len * sizeof(char*);
    char* buf = malloc(buflen);
    *(int64_t*)buf = len;
    *((int64_t*)buf + 1) = len;
    char** items = (char**)(buf + header_size);
    for (int64_t i = 0; i < len; i++) {
        char* ch = malloc(2);
        ch[0] = s[i]; ch[1] = '\0';
        items[i] = ch;
    }
    if (_cached_chars) free(_cached_chars);
    _cached_source = s;
    _cached_len = len;
    _cached_chars = buf;
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
    free(copy);
    *(int64_t*)buf = count;
    return buf;
}

void* slate_or(void* v, void* def) { return v == NULL ? def : v; }
int8_t slate_exists(void* v) { return v != NULL ? 1 : 0; }

char* file(char* path) { return path; }
char* home() { char* h = getenv("HOME"); return h ? h : ""; }

char* slate_file_read(char* path) {
    FILE* f = fopen(path, "r");
    if (!f) return "";
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    if (len < 0) { fclose(f); return ""; }
    rewind(f);
    char* buf = malloc(len + 1);
    if (!buf) { fclose(f); return ""; }
    size_t read = fread(buf, 1, len, f);
    buf[read] = '\0';
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

char* read() {
    char* buf = malloc(4096);
    if (!buf) return "";
    if (!fgets(buf, 4096, stdin)) buf[0] = '\0';
    int l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
    return buf;
}

char* read_n(int64_t n) {
    if (n < 0 || n > SIZE_MAX - 1) return "";
    char* buf = malloc(n + 1);
    if (!buf) return "";
    size_t got = fread(buf, 1, (size_t)n, stdin);
    buf[got] = '\0';
    return buf;
}

void slate_write(char* s) {
    fputs(s, stdout);
    fflush(stdout);
}

void run(char* cmd, char* a1, char* a2, char* a3, char* a4, char* a5) {
    char buf[8192];
    int pos = 0;
    size_t rem = sizeof(buf);
    if (cmd && cmd[0]) {
        int written = snprintf(buf, rem, "%s", cmd);
        if (written < 0 || (size_t)written >= rem) return;
        pos = written; rem -= written;
    }
    if (a1 && a1[0]) {
        int written = snprintf(buf + pos, rem, " %s", a1);
        if (written < 0 || (size_t)written >= rem) return;
        pos += written; rem -= written;
    }
    if (a2 && a2[0]) {
        int written = snprintf(buf + pos, rem, " %s", a2);
        if (written < 0 || (size_t)written >= rem) return;
        pos += written; rem -= written;
    }
    if (a3 && a3[0]) {
        int written = snprintf(buf + pos, rem, " %s", a3);
        if (written < 0 || (size_t)written >= rem) return;
        pos += written; rem -= written;
    }
    if (a4 && a4[0]) {
        int written = snprintf(buf + pos, rem, " %s", a4);
        if (written < 0 || (size_t)written >= rem) return;
        pos += written; rem -= written;
    }
    if (a5 && a5[0]) {
        int written = snprintf(buf + pos, rem, " %s", a5);
        if (written < 0 || (size_t)written >= rem) return;
        pos += written; rem -= written;
    }
    system(buf);
}

char* toml(char* source) {
    return slate_table_new();
}

extern void slate_main();

int main(int argc, char** argv) {
    slate_init_args(argc - 1, argv + 1);
    slate_main();
    return 0;
}