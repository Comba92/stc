#ifndef STC_STR_H
#define STC_STR_H

#include "stc_list.h"
#include "stc_mem.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

slice_define(char, str);
list_define(str, StrList);
list_define(int, IntList);

typedef int (*CharPredicate)(int);
// typedef bool (*CharPredicate)(char);


// THIS NEEDS TO BE FREED!
char* cstr(str s) {
    char* res = malloc(s.len + 1);
    foreach(char, s, c, { res[i] = *c; })
    return res;
}

str slice_from(char* s) {
    str res = {
        .data = s,
        .len = strlen(s)
    };
    return res;
}

str slice_empty() {
    return (str) {
        .data = "",
        .len = 0
    };
}

str slice_new(char* s, int start, int end) {
    assert("[slice_new]: start and end must be positive" && start >= 0 && end >= 0);
    assert("[slice_new]: start must be smaller than end" && start <= end);
    
    int len = strlen(s);
    start = MIN(start, len);
    end = MIN(end, len);

    str res = {
        .data = s + start,
        .len = end - start,
    };

    return res;
}

bool str_eq(str a, str b) {
    if (a.len != b.len) return false;
    for(size_t i=0; i<a.len; ++i) {
        if (a.data[i] != b.data[i]) return false;
    }

    return true;
}

bool str_eq_ignorecase(str a, str b) {
    if (a.len != b.len) return false;
    for(size_t i=0; i<a.len; ++i) {
        if (tolower(a.data[i]) != tolower(b.data[i])) return false;
    }

    return true;
}

int str_cmp(str a, str b) {
    if (a.len < b.len) {
        str tmp = a;
        a = b;
        b = a;
    }
    
    for(int i=0; i<a.len && i<b.len; ++i) {
        if (a.data[i] != b.data[i]) {
            return a.data[i] - b.data[i];
        }
    }

    return 0;
}


int str_find(str s, char c) {
    int idx;
    list_find(char, s, c, idx);
    return idx;
}

bool str_contains(str s, char c) {
    return str_find(s, c) != -1;
}

size_t str_match_while(str s, CharPredicate p) {
    size_t idx;
    foreach(char, s, c, { if (p(*c)) { idx = i; break; } })
    return idx;
}

size_t str_match(str s, str target) {
    str window = slice_new(s.data, 0, target.len);

    for(int i=0; i + target.len <= s.len; ++i) {
        if (str_eq(window, target)) return i;
        window.data += 1;
    }

    return -1;
}

IntList str_match_all(str s, str target) {
    IntList matches = {0};

    str window = slice_new(s.data, 0, target.len);

    for (int i=0; i + target.len <= s.len; ++i) {
        if (str_eq(window, target)) {
            list_push(matches, i);
        }

        window.data += 1;
    }

    return matches;
}

str str_skip(str s, size_t n) {
    return slice_new(s.data, n, s.len);
}
str str_skip_rev(str s, size_t n) {
    if (n > s.len) return slice_empty();
    return slice_new(s.data, 0, s.len - n);
}

str str_skip_while(str s, CharPredicate p) {
    size_t match = str_match_while(s, p);
    if (match == -1) return slice_empty();
    return str_skip(s, match);
}

str str_skip_rev_while(str s, CharPredicate p);

str str_skip_until_char(str s, char c) {
    size_t idx = str_find(s, c);
    return str_skip(s, idx);
}
str str_skip_until_match(str s, str target) {
    size_t idx = str_match(s, target);
    return str_skip(s, idx);
}

str str_take(str s, size_t n) {
    return slice_new(s.data, 0, n);
}
str str_take_rev(str s, size_t n) {
    if (n > s.len) return s;
    return slice_new(s.data, s.len - n, s.len);
}

str str_take_while(str s, CharPredicate p) {
    size_t match = str_match_while(s, p);
    if (match == -1) return s;
    return str_take(s, match);
}
str str_take_rev_while(str s, CharPredicate p);

str str_take_until_char(str s, char c) {
    size_t idx = str_find(s, c);
    return str_take(s, idx);
}
str str_take_until_match(str s, str target) {
    size_t idx = str_match(s, target);
    return str_take(s, idx);
}

bool str_starts_with(str s, str start) {
    str prefix = slice_new(s.data, 0, start.len);
    return str_eq(prefix, start);
}
bool str_ends_with(str s, str end) {
    str postfix = slice_new(s.data, s.len - end.len, s.len);
    return str_eq(postfix, end);
}

str str_trim_start(str s) {
    return str_skip_while(s, isblank);
}
str str_trim_end(str s) {
    return str_skip_rev_while(s, isblank);
}
str str_trim(str s) {
    return str_trim_end(str_trim_start(s));
}

int str_parse_int(str s) {
    char* buf = cstr(s);
    int n = atoi(buf);
    free(buf);
    return n;
}

double str_parse_float(str s) {
    char* buf = cstr(s);
    double n = atof(buf);
    free(buf);
    return n;
}

StrList str_split(str s, char c) {
    StrList ss = {0};

    while (s.len > 0) {
        int i = str_find(s, c);
        list_push(ss, slice_new(s.data, 0, i));
        s = slice_new(s.data, i+1, s.len);
    }

    return ss;
}
StrList str_lines(str s) {
    return str_split(s, '\n');
}

list_define(char, String);

// Converts a String to a slice inplace
#define STR(S) *((str*) (&(S)));

str str_slice(String s) {
    return slice_new(s.data, 0, s.len);
}

void str_append(Arena* a, String this, str other) {
    foreach(char, s, c, { arena_list_push(a, res, *c); })
}

String str_new(Arena* a, str s) {
    String res = {0};
    str_append(a, res, s);
    return res;
}

String str_with_cap(Arena* a, str s, size_t cap) {
    String res = {0};
    res.data = arena_alloc(a, cap);
    res.capacity = cap;
    str_append(a, res, s);
    return res;
}

String str_copy(Arena* a, str s);
String str_to_upper(Arena* a, str s);
String str_to_lower(Arena* a, str s);
String str_replace(Arena* a, str s, str from, str to);
String str_repeat(Arena* a, str s, size_t n);
String str_concat(Arena* a, str this, str other);

String str_join(Arena* a, StrList strs, char c);

#endif