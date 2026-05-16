#ifndef SLATE_PARSERS_H
#define SLATE_PARSERS_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// ── Minimal TOML parser (for slate.toml) ─────────────────────────────────────
// Parses section-qualified keys like "project.name", "project.version" etc.
// Returns a table(string, string).

static inline void* slate_toml_parse(const char* source) {
    void* table = slate_table_new();
    if (!source) return table;
    char* section = "";
    char* lines = strdup(source);
    char* line = lines;
    char* next;
    while (line && *line) {
        next = strchr(line, '\n');
        if (next) { *next = 0; next++; }
        // Trim
        char* s = line;
        while (*s == ' ' || *s == '\t' || *s == '\r') s++;
        char* e = s + strlen(s);
        while (e > s && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\r')) e--;
        if (e <= s) { line = next; continue; }
        *e = 0;
        // Section header
        if (*s == '[') {
            s++;
            char* bracket = strchr(s, ']');
            if (bracket) { *bracket = 0; section = s; }
            line = next;
            continue;
        }
        // Comment
        if (*s == '#') { line = next; continue; }
        // key = value
        char* eq = strchr(s, '=');
        if (eq) {
            *eq = 0;
            char* k = s;
            while (*k == ' ' || *k == '\t') k++;
            char* ke = eq - 1;
            while (ke > k && (*ke == ' ' || *ke == '\t')) ke--;
            *(ke + 1) = 0;
            char* v = eq + 1;
            while (*v == ' ' || *v == '\t' || *v == '"') v++;
            char* ve = v + strlen(v) - 1;
            while (ve > v && (*ve == ' ' || *ve == '\t' || *ve == '"')) ve--;
            *(ve + 1) = 0;
            char full_key[256];
            if (*section) {
                snprintf(full_key, sizeof(full_key), "%s.%s", section, k);
            } else {
                snprintf(full_key, sizeof(full_key), "%s", k);
            }
            table = slate_table_set(table, strdup(full_key), strdup(v));
        }
        line = next;
    }
    free(lines);
    return table;
}

// ── Minimal JSON parser ──────────────────────────────────────────────────────
// Recursive-descent parser that returns table(string, string) with dotted-key
// paths, matching the TOML parser pattern. Handles objects, arrays, strings,
// numbers, booleans, and null.

static inline void _json_skip_ws(const char* s, int* pos) {
    while (s[*pos] == ' ' || s[*pos] == '\t' || s[*pos] == '\n' || s[*pos] == '\r') (*pos)++;
}

static inline char* _json_parse_string(const char* s, int* pos) {
    (*pos)++;
    int start = *pos;
    int i = start;
    while (s[i] && s[i] != '"') {
        if (s[i] == '\\' && s[i+1]) i++;
        i++;
    }
    char* result = malloc((size_t)(i - start + 1));
    int ri = 0;
    int j = start;
    while (j < i) {
        if (s[j] == '\\' && j + 1 < i) {
            j++;
            switch (s[j]) {
                case '"':  result[ri++] = '"';  break;
                case '\\': result[ri++] = '\\'; break;
                case '/':  result[ri++] = '/';  break;
                case 'b':  result[ri++] = '\b'; break;
                case 'f':  result[ri++] = '\f'; break;
                case 'n':  result[ri++] = '\n'; break;
                case 'r':  result[ri++] = '\r'; break;
                case 't':  result[ri++] = '\t'; break;
                case 'u': {
                    if (s[j+1] && s[j+2] && s[j+3] && s[j+4]) {
                        char hex[5] = {s[j+1], s[j+2], s[j+3], s[j+4], 0};
                        unsigned long code = strtoul(hex, NULL, 16);
                        if (code < 128) result[ri++] = (char)code;
                        j += 4;
                    }
                    break;
                }
                default: result[ri++] = s[j]; break;
            }
        } else {
            result[ri++] = s[j];
        }
        j++;
    }
    result[ri] = 0;
    *pos = i + 1;
    return result;
}

// Forward declarations for mutual recursion
static inline void _json_parse_value(const char* s, int* pos, void* table, const char* prefix);
static inline void _json_parse_object(const char* s, int* pos, void* table, const char* prefix);
static inline void _json_parse_array(const char* s, int* pos, void* table, const char* prefix);

static inline void _json_parse_value(const char* s, int* pos, void* table, const char* prefix) {
    _json_skip_ws(s, pos);
    char c = s[*pos];
    if (c == '{') {
        _json_parse_object(s, pos, table, prefix);
    } else if (c == '[') {
        _json_parse_array(s, pos, table, prefix);
    } else if (c == '"') {
        char* val = _json_parse_string(s, pos);
        if (prefix && prefix[0]) slate_table_set(table, strdup(prefix), val);
        else free(val);
    } else if (c == 't') {
        *pos += 4;
        if (prefix && prefix[0]) slate_table_set(table, strdup(prefix), strdup("true"));
    } else if (c == 'f') {
        *pos += 5;
        if (prefix && prefix[0]) slate_table_set(table, strdup(prefix), strdup("false"));
    } else if (c == 'n') {
        *pos += 4;
        if (prefix && prefix[0]) slate_table_set(table, strdup(prefix), strdup("null"));
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        int num_start = *pos;
        if (s[*pos] == '-') (*pos)++;
        while (s[*pos] >= '0' && s[*pos] <= '9') (*pos)++;
        if (s[*pos] == '.') { (*pos)++; while (s[*pos] >= '0' && s[*pos] <= '9') (*pos)++; }
        if (s[*pos] == 'e' || s[*pos] == 'E') {
            (*pos)++; if (s[*pos] == '+' || s[*pos] == '-') (*pos)++;
            while (s[*pos] >= '0' && s[*pos] <= '9') (*pos)++;
        }
        int num_len = *pos - num_start;
        char* num_str = malloc((size_t)(num_len + 1));
        memcpy(num_str, s + num_start, (size_t)num_len);
        num_str[num_len] = 0;
        if (prefix && prefix[0]) slate_table_set(table, strdup(prefix), num_str);
        else free(num_str);
    }
}

static inline void _json_parse_object(const char* s, int* pos, void* table, const char* prefix) {
    (*pos)++;
    _json_skip_ws(s, pos);
    if (s[*pos] == '}') { (*pos)++; return; }
    while (1) {
        _json_skip_ws(s, pos);
        char* key = _json_parse_string(s, pos);
        _json_skip_ws(s, pos);
        if (s[*pos] == ':') (*pos)++;
        _json_skip_ws(s, pos);
        char full_key[1024];
        if (prefix && prefix[0]) snprintf(full_key, sizeof(full_key), "%s.%s", prefix, key);
        else                     snprintf(full_key, sizeof(full_key), "%s", key);
        free(key);
        _json_parse_value(s, pos, table, full_key);
        _json_skip_ws(s, pos);
        if (s[*pos] == '}') { (*pos)++; return; }
        if (s[*pos] == ',') { (*pos)++; }
    }
}

static inline void _json_parse_array(const char* s, int* pos, void* table, const char* prefix) {
    (*pos)++;
    _json_skip_ws(s, pos);
    if (s[*pos] == ']') { (*pos)++; return; }
    int index = 0;
    while (1) {
        _json_skip_ws(s, pos);
        char elem_key[1024];
        if (prefix && prefix[0]) snprintf(elem_key, sizeof(elem_key), "%s.%d", prefix, index);
        else                     snprintf(elem_key, sizeof(elem_key), "%d", index);
        _json_parse_value(s, pos, table, elem_key);
        index++;
        _json_skip_ws(s, pos);
        if (s[*pos] == ']') { (*pos)++; return; }
        if (s[*pos] == ',') { (*pos)++; }
    }
}

static inline void* slate_json_parse(const char* source) {
    void* table = slate_table_new();
    if (!source) return table;
    int pos = 0;
    _json_skip_ws(source, &pos);
    if (source[pos] == '{') _json_parse_object(source, &pos, table, "");
    else if (source[pos] == '[') _json_parse_array(source, &pos, table, "");
    return table;
}

#endif // SLATE_PARSERS_H
