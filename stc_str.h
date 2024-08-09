// ------------------------------------------------------ //
// -----------------------Strings------------------------ //
// ------------------------------------------------------ //

#ifndef STC_STR_H
#define STC_STR_H

#include "stc.h"
#include <ctype.h>
#include <string.h>
#include <assert.h>
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

// Slice, or StringView. Lightweight way to reference an allocated or static String with a length.
typedef struct CharSlice {
    char* data;
    size_t len;
} Str;

list_define(Str, StrList);
list_define(int, IntList);

typedef int (*CharPredicate) (int);
typedef bool (*StrPredicate) (Str);

// Returns an empty slice, pointing to null.
Str slice_empty() {
    Str res = {};
    res.data = NULL;
    res.len = 0;
    return res;
}

// Returns a new heap-allocated ZString, which must be freed.
char* cstr(Str s) {
    char* ptr = malloc(sizeof(char) * (s.len+1));
    foreach(char, s, c, {ptr[i] = *c; });
    ptr[s.len] = '\0';
    return ptr;
}

void str_msg(char* msg, Str s) {
    char* dbg = cstr(s);
    printf("%s > \'%s\'\n", msg, dbg);
    free(dbg);
}

void str_dbg(Str s) {
    str_msg("", s);
}

//TODO: size_t is unsigned, so if you pass a negative number for any reason, KABOOM! Be careful.
Str slice_new(char* s, size_t start, size_t end) {
    assert("created slice with negative indexes" && ((int) start) >= 0 && ((int) end) >= 0 );

    if (start >= end) { return slice_empty(); };

    Str res = {};
    start = MAX(0, start);
    res.data = s + start;
    res.len = end - start;
    return res;
}

// Returns a slice from a ZString.
Str str(char* s) {
    return slice_new(s, 0, strlen(s));
}

Str str_substr(Str s, size_t start, size_t end) {
    return slice_new(s.data, start, end);
}

char str_get(Str s, int i) {
    if (i > s.len) return '\0';
    return s.data[i];
}

bool str_eq(Str a, Str b) {
    if (a.len != b.len) return false;

    for(size_t i=0; i<a.len; ++i) {
        if (a.data[i] != b.data[i]) return false;
    }

    return true;
}

bool str_eq_ignore_case(Str a, Str b) {
    if (a.len != b.len) return false;
    for(size_t i=0; i<a.len; ++i) {
        if (tolower(a.data[i]) != tolower(b.data[i])) return false;
    }

    return true;
}

int str_cmp(Str a, Str b) {
    if (a.len < b.len) {
        Str temp = a;
        a = b;
        b = temp;
    }

    for (size_t i=0; i<a.len; ++i) {
        if (a.data[i] != b.data[i]) return a.data[i] - b.data[i];
    }

    return 0;
}

int str_find(Str s, char target) {
    foreach(char, s, c, { if (*c == target) return i; });
    return -1;
}

bool str_contains(Str s, char target) {
    return str_find(s, target) != -1;
}


//TODO: a lot of repetitions with take and chop, maybe factor out to a generic function?

Str str_take_left(Str s, size_t len) {
    return str_substr(s, 0, len);
}

Str str_take_right(Str s, size_t len) {
    if (len > s.len) return s;
    return str_substr(s, s.len - len, s.len);
}

Str str_take_while(Str s, CharPredicate p) {
    size_t len = 0;
    foreach(char, s, c, { if (p(*c)) { break; } len+=1; });
    return str_take_left(s, len);
}

Str str_take_until_match(Str s, Str target) {
    Str window = str_substr(s, 0, target.len);
    size_t i = 0;
    for (; i + target.len <= s.len; ++i) {
        if (str_eq(window, target)) return str_take_left(s, i);
        window.data += 1;
    }

    return s;
}

// TODO: make these better
Str str_take_until_char(Str s, char c) {
    char* tmp = malloc(sizeof(char)+1);
    tmp[0] = c; tmp[1] = '\0';
    Str res = str_take_until_match(s, str(tmp));
    free(tmp);
    return res;
}

Str str_chop_left(Str s, size_t len) {
    return str_substr(s, len, s.len);
}

Str str_chop_right(Str s, size_t len) {
    if (len > s.len) return slice_empty();
    return str_substr(s, 0, s.len - len);
}

Str str_chop_until_match(Str s, Str target) {
    Str window = str_substr(s, 0, target.len);
    size_t i = 0;
    for (; i + target.len <= s.len; ++i) {
        if (str_eq(window, target)) return str_chop_left(s, i + target.len);
        window.data += 1;
    }

    return slice_empty();
}

//TODO: make this better
Str str_chop_until_char(Str s, char c) {
    char* tmp = malloc(sizeof(char)+1);
    tmp[0] = c; tmp[1] = '\0';
    Str res = str_chop_until_match(s, str(tmp));
    free(tmp);
    return res;
}


Str str_take_left_while(Str s, CharPredicate p) {
    size_t i = 0;
    while(i < s.len && p(s.data[i])) i += 1;
    return str_take_left(s, i);
}

//TODO: make these better
Str str_take_right_while(Str s, CharPredicate p) {
    size_t i = s.len-1;
    while(i >= 0 && p(s.data[i])) i -= 1;
    return str_take_right(s, MAX(0, ((int) s.len) - i - 1));
}

Str str_chop_left_while(Str s, CharPredicate p) {
    size_t i = 0;
    while(i < s.len && p(s.data[i])) i += 1;
    return str_chop_left(s, i);
}

//TODO: make these better
Str str_chop_right_while(Str s, CharPredicate p) {
    int i = s.len-1;
    while(i >= 0 && p(s.data[i])) i -= 1;
    return str_chop_right(s, MAX(0, ((int) s.len) - i - 1));
}

Str str_trim_start(Str s) {
    return str_chop_left_while(s, isspace);
}

Str str_trim_end(Str s) {
    return str_chop_right_while(s, isspace);
}

Str str_trim(Str s) {
    return str_trim_end(str_trim_start(s));
}

bool str_starts_with(Str s, Str target) {
    return str_eq(str_take_left(s, target.len), target);
}

bool str_ends_with(Str s, Str target) {
    return str_eq(str_take_right(s, target.len), target);
}

int str_match(Str s, Str target) {
    Str window = str_substr(s, 0, target.len);

    for (int i=0; i + target.len <= s.len; ++i) {
        if (str_eq(window, target)) {
            return i;
        }

        window.data += 1;
    }

    return -1;
}

IntList str_match_all(Str s, Str target) {
    IntList matches = {0};

    Str window = str_substr(s, 0, target.len);

    for (int i=0; i + target.len <= s.len; ++i) {
        if (str_eq(window, target)) {
            list_push(matches, i);
        }

        window.data += 1;
    }

    return matches;
}

StrList str_split(Str s, char split) {
    StrList res = {0};

    while (s.len > 0) {
        Str taken = str_take_until_char(s, split);
        list_push(res, taken);
        Str chopped = str_chop_until_char(s, split);
        s = chopped;
    }
    
    return res;
}

StrList str_lines(Str s) {
    StrList sl = str_split(s, '\n');
    return sl;
}

// === HEAP STRINGS === //

// Heap-allocated String. Can be edited and appended. Must be freed.
typedef struct CharList {
    char* data;
    size_t len;
    size_t capacity;
    /*  Slice for easy access. Must be set every time a succession of pushes is done,
     *  as it might realloc the string, changing its address.
     */
    Str slice;
} String;

// Returns a slice from an owned String. This value is also hold in the slice field.
Str slice_from_str(String s) {
    // easy way: 
    // return str_new(s.data, 0, s.len);

    // Get String address, cast to Slice address, derefence it
    return *((Str*) (&s));
}

String str_from_cstring(char* s) {
    String res = {0};
    int length = strlen(s);
    
    for(int i=0; i<length; ++i) {
        list_push(res, s[i]);
    }

    res.slice = slice_from_str(res);
    return res;
}

String str_from_slice(Str s) {
    String res = {0};
    foreach(char, s, c, { list_push(res, *c); });

    res.slice = slice_from_str(res);
    return res;
}

void str_free(String* s) {
    list_free(*s);
}

//TODO: convert all String arguments to slices?
String str_copy(String s) {
    String res = {0};
    foreach(char, s, c, { list_push(res, *c); });

    res.slice = slice_from_str(res);
    return res;
}

String str_append(String src, Str other) {
    String res = str_copy(src);
    list_concat(char, res, other);

    res.slice = slice_from_str(res);
    return res;
}

String str_insert(String src, size_t start, Str other) {
    String res = {0};
    size_t i = 0;
    while (i < start) { list_push(res, src.data[i++]); }
    foreach(char, other, c, { list_push(res, *c); });
    while (i < src.len) { list_push(res, src.data[i++]); }

    res.slice = slice_from_str(res);
    return res; 
}

String str_remove(String src, size_t start, size_t len) {
    String res = {0};
    size_t i = 0;
    while (i < start) { list_push(res, src.data[i++]); }
    i += len;
    while (i < src.len) { list_push(res, src.data[i++]); }

    res.slice = slice_from_str(res);
    return res;
}

bool str_set(String* dst, char c, int i) {
    if (i > dst->len) return false; 
    dst->data[i] = c;
    return true;
}

// edits in-place a string after start. will edit until s doesn't fit
bool str_edit(String* dst, int start, Str s)  {
    if (start > dst->len) return false;
    for(size_t i=start; i < dst->len; ++i) {
        dst->data[i] = s.data[i - start];
    }

    return true;
}

// edits in-place a string after start. will edit until s doesn't fit
bool str_erase(String* dst, int start, int len)  {
    if (start + len > dst->len) return false;

    //TODO: implement this

    return false;
}

String str_map(Str s, CharPredicate p) {
    String res = {0};
    foreach(char, s, c, { *c = (char) p((int)*c); list_push(res, *c); });
    
    res.slice = slice_from_str(res);
    return res;
}

String str_toupper(Str s) {
    return str_map(s, &toupper);
}

String str_tolower(Str s) {
    return str_map(s, &tolower);
}

String str_repeat(Str s, size_t reps) {
    String res = {0};
    
    for (int i=0; i<reps; ++i) {
        String new_res = str_append(res, s);
        str_free(&res);
        res = new_res;
    }

    res.slice = slice_from_str(res);
    return res;
}

String str_replace(String s, Str target, Str replacing) {
    int match = str_match(s.slice, target);
    if (match == -1) return s;

    String removed = str_remove(s, match, target.len);
    String replaced = str_insert(removed, match, replacing);
    str_free(&removed);

    replaced.slice = slice_from_str(replaced);
    return replaced;
}

String str_replace_all(String s, Str target, Str replacing) {
    String res = str_copy(s);
    String tmp = {0};

    // Replace one at a time, stop when the last replace hasn't edited the string
    while(!str_eq(res.slice, slice_from_str(tmp))) {
        String replaced = str_replace(res, target, replacing);
        tmp = res;
        res = replaced;
    }

    str_free(&tmp);
    res.slice = slice_from_str(res);
    return res;
}

String str_join(StrList sl, char join) {
    String res = {0};

    foreach(Str, sl, s, {
        list_concat(char, res, *s);
        list_push(res, join);
    });

    // pop last char
    list_pop(res);

    res.slice = slice_from_str(res);
    return res;
}

#endif