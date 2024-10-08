#include <stdio.h>
#include "stc_str.h"

int main() {
    String s = str_new("Hello World!");
    printf("String of len %ld says %s\n", str_len(s), s);
}