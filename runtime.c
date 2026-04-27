#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// ─── String operations ───────────────────────────────────────────────────────
char* slate_string_concat(char* a, char* b) {
    int la = strlen(a), lb = strlen(b);
    char* buf = malloc(la + lb + 1);
    memcpy(buf, a, la); memcpy(buf + la, b, lb);
    buf[la + lb] = '\0'; return buf;
}

char* slate_string(int64_t n) { char* buf = malloc(32); snprintf(buf, 32, "%lld", (long long)n); return buf; }
int64_t slate_int(char* s) { return (int64_t)atoll(s); }
double slate_float(char* s) { return atof(s); }

// ─── Print ───────────────────────────────────────────────────────────────────
void slate_print(char* s) { puts(s); }

// ─── List (ptr array with length prefix) ─────────────────────────────────────
char* slate_list_new() { char* buf = malloc(sizeof(int64_t)); *(int64_t*)buf = 0; return buf; }
int64_t slate_len(char* p) { return *(int64_t*)p; }
char* slate_get(char* p, int64_t index) {
    int64_t len = *(int64_t*)p;
    if (index < 0 || index >= len) { fprintf(stderr, "panic: index out of bounds\n"); exit(1); }
    char** items = (char**)(p + sizeof(int64_t)); return items[index];
}
char* slate_add(char* p, char* val) {
    int64_t len = *(int64_t*)p, new_len = len + 1;
    char* buf = malloc(sizeof(int64_t) + new_len * sizeof(char*));
    *(int64_t*)buf = new_len;
    char** items = (char**)(buf + sizeof(int64_t));
    char** old_items = (char**)(p + sizeof(int64_t));
    for (int64_t i = 0; i < len; i++) items[i] = old_items[i];
    items[len] = val; return buf;
}

// ─── Table ──────────────────────────────────────────────────────────────────
char* slate_table_new() { char* buf = malloc(sizeof(int64_t)); *(int64_t*)buf = 0; return buf; }
int64_t slate_table_len(char* p) { return *(int64_t*)p; }
int8_t slate_has(char* p, char* key) {
    int64_t len = *(int64_t*)p;
    char** keys = (char**)(p + sizeof(int64_t));
    for (int64_t i = 0; i < len; i++) if (strcmp(keys[i], key) == 0) return 1;
    return 0;
}
char* slate_get_table(char* p, char* key) {
    int64_t len = *(int64_t*)p;
    char** keys = (char**)(p + sizeof(int64_t));
    char** vals = (char**)(p + sizeof(int64_t) + len * sizeof(char*));
    for (int64_t i = 0; i < len; i++) if (strcmp(keys[i], key) == 0) return vals[i];
    return "";
}
char* slate_set(char* p, char* key, char* val) {
    int64_t len = *(int64_t*)p, new_len = len + 1;
    char* buf = malloc(sizeof(int64_t) + new_len * sizeof(char*) * 2);
    *(int64_t*)buf = new_len;
    char** new_keys = (char**)(buf + sizeof(int64_t));
    char** new_vals = (char**)(buf + sizeof(int64_t) + new_len * sizeof(char*));
    char** keys = (char**)(p + sizeof(int64_t));
    char** vals = (char**)(p + sizeof(int64_t) + len * sizeof(char*));
    for (int64_t i = 0; i < len; i++) { new_keys[i] = keys[i]; new_vals[i] = vals[i]; }
    new_keys[len] = key; new_vals[len] = val; return buf;
}

// ─── Input / args ─────────────────────────────────────────────────────────────
static char** _slate_argv = NULL; static int64_t _slate_argc = 0;
void slate_init_args(int argc, char** argv) { _slate_argc = argc; _slate_argv = argv; }
char* slate_args(char* handle) {
    char* buf = malloc(sizeof(int64_t) + _slate_argc * sizeof(char*));
    *(int64_t*)buf = _slate_argc;
    char** items = (char**)(buf + sizeof(int64_t));
    for (int64_t i = 0; i < _slate_argc; i++) items[i] = _slate_argv[i];
    return buf;
}

// ─── String methods ────────────────────────────────────────────────────────────
char* slate_chars(char* s) {
    int64_t len = strlen(s);
    char* buf = malloc(sizeof(int64_t) + len * sizeof(char*));
    *(int64_t*)buf = len;
    char** items = (char**)(buf + sizeof(int64_t));
    for (int64_t i = 0; i < len; i++) { char* ch = malloc(2); ch[0] = s[i]; ch[1] = '\0'; items[i] = ch; }
    return buf;
}
int8_t slate_contains(char* h, char* n) { return strstr(h, n) != NULL ? 1 : 0; }
int8_t slate_starts_with(char* s, char* p) { return strncmp(s, p, strlen(p)) == 0 ? 1 : 0; }
int8_t slate_ends_with(char* s, char* suf) { int sl=strlen(s), fl=strlen(suf); return fl>sl?0:strcmp(s+sl-fl,suf)==0?1:0; }
char* slate_replace(char* s, char* from, char* to) {
    char* result = malloc(strlen(s) * 2 + 1);
    char* pos = strstr(s, from);
    if (!pos) { strcpy(result, s); return result; }
    int from_len = strlen(from), to_len = strlen(to), prefix_len = pos - s;
    memcpy(result, s, prefix_len); memcpy(result + prefix_len, to, to_len);
    strcpy(result + prefix_len + to_len, pos + from_len); return result;
}
char* slate_split(char* s, char* delim) {
    int dlen = strlen(delim), slen = strlen(s);
    char* buf = malloc(sizeof(int64_t) + slen * sizeof(char*));
    *(int64_t*)buf = 0;
    int count = 0;
    char** items = (char**)(buf + sizeof(int64_t));
    char* copy = malloc(slen + 1); strcpy(copy, s); char* cur = copy;
    while (1) {
        char* found = dlen > 0 ? strstr(cur, delim) : NULL;
        char* part;
        if (!found) { part = malloc(strlen(cur) + 1); strcpy(part, cur); items[count++] = part; break; }
        int plen = found - cur;
        part = malloc(plen + 1); memcpy(part, cur, plen); part[plen] = '\0'; items[count++] = part;
        cur = found + dlen; if (dlen == 0 && *cur == '\0') break;
    }
    *(int64_t*)buf = count; return buf;
}

// ─── Option methods ───────────────────────────────────────────────────────────
void* slate_or(void* v, void* def) { return v == NULL ? def : v; }
int8_t slate_exists(void* v) { return v != NULL ? 1 : 0; }

// ─── Builtins matching IR call names ───────────────────────────────────────
char* file(char* path) { return path; }
char* home() { char* h = getenv("HOME"); return h ? h : ""; }
void run_fn(char* cmd) { system(cmd); }
char* read_n(int64_t n) { char* b = malloc(n+1); fread(b,1,n,stdin); b[n]=0; return b; }
char* read_line() { char* b = malloc(4096); if(!fgets(b,4096,stdin))b[0]=0; int l=strlen(b); if(l>0&&b[l-1]=='\n')b[l-1]=0; return b; }
void write_stdout(char* s) { fputs(s,stdout); fflush(stdout); }

// ─── Ink color functions (ANSI) ───────────────────────────────────────────
static char* ansi_wrap(const char* code, char* text) {
    int len = strlen(text), code_len = strlen(code);
    char* buf = malloc(code_len + len + 5);
    sprintf(buf, "%s%s\033[0m", code, text); return buf;
}