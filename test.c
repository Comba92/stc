#include <stdio.h>
#include "stc.h"
#include "stc_str.h"

map_define(int, IntMap);

int is_bang(int c) { return c != '!'; }
int is_h(int c) { return c != 'H'; }
int is_a(int c) { return c == 'a'; }

int main() {
    str_dbg(str("Fag"));
    Str hello = str("Hello!!");
    Str world = str("World.");
    str_dbg(str_substr(hello, 2, 2));
    str_dbg(str_substr(hello, 6, 2));
    str_dbg(str_substr(hello, 2, 100));
    

    assert(str_eq(hello, str("Hello!!")));
    assert(str_eq_ignore_case(hello, str("hello!!")));

    printf("%d\n", str_cmp(hello, world));
    printf("%d\n", str_find(hello, '!'));
    
    assert(str_contains(hello, '!'));
    str_msg("Take l", str_take_left(hello, 3));
    str_msg("Take l", str_take_left(hello, 100));
    str_msg("Take r", str_take_right(hello, 3));
    str_msg("Take r", str_take_right(hello, 100));

    str_msg("Chop l", str_chop_left(hello, 3));
    str_msg("Chop l", str_chop_left(hello, 100));
    str_msg("Chop r", str_chop_right(hello, 2));
    str_msg("Chop r", str_chop_right(hello, 100));

    str_dbg(str_chop_left_while(hello, is_bang));
    str_dbg(str_chop_right_while(hello, is_bang));

    Str totrim = str("       dsa asadsa         ");
    str_dbg(str_trim(totrim));

    assert(str_starts_with(hello, str("H")));
    assert(str_ends_with(hello, str("!!")));

    str_msg("Take while", str_take_while(totrim, is_a));
    str_dbg(str_take_until_match(totrim, str("sad")));
    str_dbg(str_take_until_match(hello, str("llo")));

    Str ll = str("ll");
    Str ds = str("ds");

    str_dbg(str_match(hello, ll));
    str_dbg(str_match(hello, str("fag")));

    foreach(Str, str_matchall(totrim, ds), s, {
        str_msg("Match:", *s);
    });

    Str lines = str("kill\nyour\nself!");
    foreach(Str, str_lines(lines), s, {
        str_msg("Lines:", *s);
    });


    StrList splitted = str_split(hello, 'l');
    foreach(Str, splitted, s, {
        str_msg("Split:", *s);
    });

    return 0;
}
