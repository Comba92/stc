#include <stdio.h>
#include "stc.h"
#include "stc_str.h"

map_define(int, IntMap);

int is_not_bang(int c) { return c != '!'; }
int is_bang(int c) { return c == '!'; }
int is_not_h(int c) { return c != 'H'; }
int is_a(int c) { return c == 'a'; }

int main() {
    str_dbg(slice_empty());
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

    str_dbg(str_chop_left_while(hello, is_not_bang));
    str_dbg(str_chop_right_while(hello, is_not_bang));

    Str totrim = str("       dsa asadsa         ");
    str_msg("Trim", str_trim_start(totrim));
    str_msg("Trim", str_trim_end(totrim));

    str_msg("Trim", str_trim_start(hello));
    str_msg("Trim", str_trim_end(hello));


    assert(str_starts_with(hello, str("H")));
    assert(str_ends_with(hello, str("!!")));

    str_msg("Take while", str_take_while(totrim, is_a));
    str_msg("Take while", str_take_while(totrim, is_bang));
    str_dbg(str_take_until_match(totrim, str("sad")));
    str_dbg(str_take_until_match(hello, str("llo")));
    str_dbg(str_take_until_match(hello, str("!")));
    str_dbg(str_take_until_match(hello, str("?")));

    Str ll = str("ll");
    Str ds = str("ds");

    printf("%d\n", str_match(hello, ll));
    printf("%d\n", str_match(hello, str("fag")));

    foreach(int, str_match_all(totrim, ds), s, {
        printf("Match: %d\n", *s);
    });

    str_msg("Take until", str_take_until_char(str("he\ny!"), '\n'));
    str_msg("Take until", str_take_until_char(str("he\ny!"), '.'));
    str_msg("Chop until", str_chop_until_char(str("he\ny!"), '\n'));
    str_msg("Chop until", str_chop_until_char(str("he\ny!"), '.'));

    StrList lines = str_lines(str("kill\nyour\nself!"));

    foreach(Str, lines, s, {
        str_msg("Lines:", *s);
    });

    StrList splitted = str_split(hello, 'l');
    foreach(Str, splitted, s, {
        str_msg("Split:", *s);
    });

    String kill = str_from_slice(lines.data[0]);
    str_dbg( kill.slice );
    str_free(&kill);

    String owned = str_from_cstring("Faggot");
    str_dbg(owned.slice);
    String appended = str_append(owned, str("Killer"));
    str_dbg(appended.slice);

    String inserted = str_insert(appended, 6, str("ry"));
    str_dbg(inserted.slice);

    str_edit(&inserted, inserted.len-3, str("AAA"));
    str_dbg(inserted.slice);

    str_dbg(str_toupper(inserted.slice).slice);
    String repeated = str_repeat(owned.slice, 5);
    str_dbg(repeated.slice);

    String hello_str = str_from_slice(hello);
    str_dbg(str_replace(hello_str, str("!!"), str("Pussy")).slice);
    str_dbg(str_replace(hello_str, str("ll"), str("Pussy")).slice);
    str_dbg(str_replace(repeated, str("Faggot"), str("Pussy")).slice);
    str_msg("Replace ALL", str_replace_all(repeated, str("Faggot"), str("Pussy")).slice);
    str_msg("Replace ALL", str_replace_all(hello_str, str("l"), str("Pussy")).slice);
    str_msg("Replace ALL", str_replace_all(hello_str, str("!"), str("Pussy")).slice);

    String joined = str_join(lines, ',');
    str_dbg(joined.slice);

    return 0;
}
