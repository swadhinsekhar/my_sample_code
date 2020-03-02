/*                    # and ## Operators in C

    1. Stringizing operator (#)
        The # operator, which is generally called the stringize operator,
        turns the argument it precedes into a quoted string.

    2. Token-pasting operator (##)
        The ‘##’ pre-processing operator performs token pasting.
        When a macro is expanded, the two tokens on either side
        of each ‘##’ operator are combined into a single token,
        which then replaces the ‘##’ and the two original tokens
        in the macro expansion.
*/

#include <stdio.h>

#define to_str(s)               #s
#define token_pasting(a)        to_str(DATA_##a)
#define concate(x, y)           x##y

#define COUNT(a) {#a, 100##a}

struct {char *s; int num;} r[] = {
   COUNT(1), COUNT(2), COUNT(3)
};

int main(int argc, char **argv)
{
    int xy = 20;

    printf("%s\n", to_str(hello world));
    printf("%s\n", token_pasting(hello));

    printf("val: %d\n", concate(x, y));

    size_t i = 0;

    while (i < sizeof(r)/sizeof(r[0])) {
        printf("%s  :   %d\n", r[i].s, r[i].num);
        i++;
    }
}
