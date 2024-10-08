#include <stdio.h>
#include "stc_str.h"


void str_msg(char* msg, str s) {
    char* dbg = cstr(s);
    printf("\'%s\'\t> %s\n", dbg, msg);
    free(dbg);
}

#define str_dbg(s) str_msg(#s, s)
#define res_dbg(s) printf("%d > " #s "\n", s);

int main() {
    str s = str_from("Hello World!");
    printf("String of len %ld says %s\n", s.len, cstr(s));
    str_dbg(str_new(s.data, 0, 5));
    res_dbg(str_eq(str_from("ass"), str_from("ass")));
    res_dbg(str_eq(str_from("ass"), str_from("ass")));
    res_dbg(str_eq_ignorecase(str_from("ASS"), str_from("ass")));
    res_dbg(str_cmp(str_from("ass"), str_from("bass")));
    res_dbg(str_cmp(str_from("bass"), str_from("ass")));
    res_dbg(str_cmp(str_from("ass"), str_from("ass")));

    res_dbg(str_find(s, 'W'));
    res_dbg(str_contains(s, 'W'));
    res_dbg(str_match_while(s, isupper));
    res_dbg(str_match_while_rev(s, isupper));
    
    printf("\n");

    res_dbg(str_match(s, str_from("World")));
    res_dbg(str_match(s, str_from("!")));
    res_dbg(str_match(s, str_from("Fuck")));
    res_dbg(str_match_all(s, str_from("l")).len);
    res_dbg(str_match_all(s, str_from("ll")).len);

    str_dbg(str_skip(s, 1));
    str_dbg(str_skip(s, s.len-1));
    str_dbg(str_skip(s, s.len));


    str_dbg(str_skip_rev(s, 1));
    str_dbg(str_skip_rev(s, s.len-1));
    str_dbg(str_skip_rev(s, s.len));

    str_dbg(str_skip_while(s, ispunct));
    str_dbg(str_skip_while_rev(s, ispunct));

    str_dbg(str_skip_until_char(s, 'W'));
    str_dbg(str_skip_until_match(s, str_from("World")));

    printf("\n");

    str_dbg(str_take(s, 1));
    str_dbg(str_take(s, s.len-1));
    str_dbg(str_take(s, s.len));


    str_dbg(str_take_rev(s, 1));
    str_dbg(str_take_rev(s, s.len-1));
    str_dbg(str_take_rev(s, s.len));

    str_dbg(str_take_while(s, ispunct));
    str_dbg(str_take_while_rev(s, ispunct));

    str_dbg(str_take_until_char(s, 'W'));
    str_dbg(str_take_until_match(s, str_from("World")));

    printf("\n");

    res_dbg(str_starts_with(s, str_from("FAG")));
    res_dbg(str_starts_with(s, str_from("Hello")));

    res_dbg(str_ends_with(s, str_from("FAG")));
    res_dbg(str_ends_with(s, str_from("!")));

    str_dbg(str_trim(str_from("  d dsad das  ")));
    res_dbg(str_parse_int(str_from("1234")));
    res_dbg(str_parse_int(str_from("a1234")));
    res_dbg(str_parse_int(str_from("1234a")));

    StrList ss1 = str_split(str_from("abc,,bcd,cde,defg"), ',');
    foreach(str, ss1, s, { str_dbg(*s); })

    StrList ss2 = str_lines(str_from("abc\nbcd\ncde\ndefg"));
    foreach(str, ss2, s, { str_dbg(*s); })

    StrList ss3 = str_split_match(str_from("abcbbccbcd"), str_from("bc"));
    foreach(str, ss3, s, { str_dbg(*s); })

    printf("\n");

    Arena a = {0};
    str_dbg(STR(str_arena(&a, s)));

    String ls = str_with_cap(&a, s, DEFAULT_LIST_CAP);
    str_append(&a, ls, str_from(" kys faggot"));
    str_dbg(STR(ls));

}