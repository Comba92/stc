#ifndef STC_STR_H
#define STC_STR_H

#include "stc_list.h"
#include "stc_mem.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

slice_define(char, str);
list_define(char, String);
list_define(str, StrList);
list_define(String, StringList);

typedef int (*CharPredicate)(int);
// typedef bool (*CharPredicate)(char);
int isnotblank(int c) { return !isblank(c); }


// THIS NEEDS TO BE FREED!
char* cstr(str s) {
    char* res = malloc(s.len + 1);
    foreach(char, s, c, { res[i] = *c; })
    res[s.len] = '\0';
    return res;
}

str str_from(char* s) {
    str res = {
        .data = s,
        .len = strlen(s)
    };
    return res;
}

str str_empty() {
    return (str) {
        .data = "",
        .len = 0
    };
}

str str_slice(char* s, int start, int end) {
    assert("[str_slice]: start and end must be positive" && start >= 0 && end >= 0);
    assert("[str_slice]: start must be smaller than end" && start <= end);
    
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
    for(int i=0; i<a.len; ++i) {
        if (a.data[i] != b.data[i]) return false;
    }

    return true;
}

bool str_eq_ignorecase(str a, str b) {
    if (a.len != b.len) return false;
    for(int i=0; i<a.len; ++i) {
        if (tolower(a.data[i]) != tolower(b.data[i])) return false;
    }

    return true;
}

int str_cmp(str a, str b) {    
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

int str_match_while(str s, CharPredicate p) {
    int idx = -1;
    foreach(char, s, c, { if (p(*c)) { idx = i; break; } })
    return idx;
}

int str_match_while_rev(str s, CharPredicate p) {
    for(int i=s.len-1; i>=0; --i) {
        if (p(s.data[i])) return i;
    }
    return -1;
}

int str_match(str s, str target) {
    str window = str_slice(s.data, 0, target.len);

    for(int i=0; i + target.len <= s.len; ++i) {
        if (str_eq(window, target)) return i;
        window.data += 1;
    }

    return -1;
}

IntList str_match_all(str s, str target) {
    IntList matches = {0};

    str window = str_slice(s.data, 0, target.len);

    for (int i=0; i + target.len <= s.len; ++i) {
        if (str_eq(window, target)) {
            list_push(matches, i);
        }

        window.data += 1;
    }

    return matches;
}

str str_skip(str s, int n) {
    return str_slice(s.data, n, s.len);
}
str str_skip_rev(str s, int n) {
    if (n > s.len) return str_empty();
    return str_slice(s.data, 0, s.len - n);
}

str str_skip_while(str s, CharPredicate p) {
    int match = str_match_while(s, p);
    if (match == -1) return str_empty();
    return str_skip(s, match);
}

str str_skip_while_rev(str s, CharPredicate p) {
    int match = str_match_while_rev(s, p);
    if (match == -1) return str_empty();
    return str_skip_rev(s, s.len - match);
}

str str_skip_until_char(str s, char c) {
    int idx = str_find(s, c);
    return str_skip(s, idx);
}
str str_skip_until_match(str s, str target) {
    int idx = str_match(s, target);
    return str_skip(s, idx);
}

str str_take(str s, int n) {
    return str_slice(s.data, 0, n);
}
str str_take_rev(str s, int n) {
    if (n > s.len) return s;
    return str_slice(s.data, s.len - n, s.len);
}

str str_take_while(str s, CharPredicate p) {
    int match = str_match_while(s, p);
    if (match == -1) return s;
    return str_take(s, match);
}
str str_take_while_rev(str s, CharPredicate p) {
    int match = str_match_while_rev(s, p);
    if (match == -1) return s;
    return str_take_rev(s, s.len - match);
}

str str_take_until_char(str s, char c) {
    int idx = str_find(s, c);
    return str_take(s, idx);
}
str str_take_until_match(str s, str target) {
    int idx = str_match(s, target);
    return str_take(s, idx);
}

bool str_starts_with(str s, str start) {
    str prefix = str_slice(s.data, 0, start.len);
    return str_eq(prefix, start);
}
bool str_ends_with(str s, str end) {
    str postfix = str_slice(s.data, s.len - end.len, s.len);
    return str_eq(postfix, end);
}

str str_trim_start(str s) {
    return str_skip_while(s, isnotblank);
}
str str_trim_end(str s) {
    return str_skip_while_rev(s, isnotblank);
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

StrList str_split(str s, str pattern) {
    StrList ss = {0};

    while (s.len > 0) {
        int i = str_match(s, pattern);
        if (i == -1) {
            list_push(ss, str_slice(s.data, 0, s.len));
            break;
        }
        list_push(ss, str_slice(s.data, 0, i));
        s = str_slice(s.data, i+pattern.len, s.len);
    }

    return ss;
}

StrList str_lines(str s) {
    return str_split(s, str_from("\n"));
}



String string_with_cap(Arena* a, str s, int cap) {
    String res = {0};
    res.data = arena_alloc(a, cap);
    res.capacity = cap;
    res.len = s.len;
    memcpy(res.data, s.data, s.len);
    return res;
}

String string_from(Arena* a, str s) {
    return string_with_cap(a, s, s.len);
}

// Converts a String to a slice inplace
// #define STR(S) *((str*) (&(S)));
str STR(String S) {
    return *((str*) (&S));
}

// str str_view(String s) {
//     return str_slice(s.data, 0, s.len);
// }

void str_append(Arena* a, String* this, str other) {
    foreach(char, other, c, { arena_list_push(a, *this, *c); })
}

String str_concat(Arena* a, str this, str other) {
    String res = {0};
    str_append(a, &res, this);
    str_append(a, &res, other);
    return res;
}
String str_repeat(Arena* a, str s, int n) {
    String res = string_with_cap(a, s, s.len * n);
    n--;
    while (n > 0) {
        str_append(a, &res, s);
        n--;
    }

    return res;
}

String str_to_upper(Arena* a, str s) {
    String res = {0};
    arena_list_map(a, char, s, char, res, c, (toupper(*c)));
    return res;
}

String str_to_lower(Arena* a, str s) {
    String res = {0};
    arena_list_map(a, char, s, char, res, c, (tolower(*c)));
    return res;
}

String str_replace(Arena* a, str s, str from, str to) {
    int match = str_match(s, from);
    if (match == -1) return string_from(a, s);

    String res = {0};
    str_append(a, &res, str_slice(s.data, 0, match));
    str_append(a, &res, to);
    str_append(a, &res, str_slice(s.data, match + from.len, s.len));
    return res;
}
String str_replace_all(Arena* a, str s, str from, str to) {
    IntList matches = str_match_all(s, from);

    String res = {0};
    int last = 0;
    foreach(int, matches, match, {
        str_append(a, &res, str_slice(s.data, last, *match));
        str_append(a, &res, to);
        last = *match + from.len;
    })
    str_append(a, &res, str_slice(s.data, last, s.len));

    return res;
}

String str_join(Arena* a, StrList strs, str join) {
    String ss = {0};

    for(int i=0; i<strs.len-1; ++i) {
        str_append(a, &ss, strs.data[i]);
        str_append(a, &ss, join);
    }
    str_append(a, &ss, strs.data[strs.len-1]);

    return ss;
}

#endif