#include <stdio.h>
#include <stdint.h>

/*
 *   Find a bit:
 *       (num & (1 << pos))    or      ((num >> pos) & 1)
 *   Set a bit:
 *       (num | (1 << pos))
 *   Toggle a bit:
 *       (num ^ (1 << pos))
 *   Clear a bit:
 *       (num & ~(1 << pos))
 */

#define IS_POW2(x)      ((x) && !(x & (x - 1)))
#define IS_POW4(x)      ((x) && IS_POW2(x) && !(x & 0xAAAAAAAA))
#define IS_EVEN(x)      ((x) && !(x & 1))

int is_power_of_4(int n)
{
    int c = 0;

    if(n && IS_POW2(n)) {

        while (n > 1) {
            n >>= 1;
            c += 1;
        }
        return (c % 2 == 0)? 1 : 0;
    }

    return 0;
}

int count_bits(char x)
{
    int i, count = 0;

    for(i=0; i<8; i++) {
        if(x & (1 << i)) {
            count++;
        }
    }

    return count;
}

int count_bit(unsigned char x)
{
    int count = 0;

    while(x) {
        if(x & 1)
            count++;
        x >>= 1;
    }

    return count;
}

void reverse_bits(int a)
{
    int i, j, m, n;

    printf("Before reverse:\n");
    for(i = sizeof(int)*8-1; i>=0; printf("%d", a >> i-- & 1));
    printf("\n");

    for(i=0, j = 31; i < j; i++, j--) {
        m = a & (1<<i);
        n = a & (1<<j);
        if(m != n) {
            a = a ^ (1<<i);
            a = a ^ (1<<j);
        }
    }

    printf("After reverse:\n");
    for(i = sizeof(int)*8-1; i>=0; printf("%d", a >> i-- & 1));

    printf("\n");
}

#ifdef MAIN
int main()
{
    char    a = 55;

    printf("%d\n", count_bit(a));
    printf("%d\n", count_bits(a));

    int num = 1024;
    reverse_bits(num);

    int x = 16;
    printf("%d is powerof2 : (%s)\n", x, (IS_POW2(x))? "true": "false");
    printf("%d is even : (%s)\n", x, (IS_EVEN(x))? "true": "false");
    printf("%d is power of 4 : (%s)\n", x, (is_power_of_4(x))? "true": "false");
    printf("%d is power of 4 macro : (%s)\n", x, (IS_POW4(x))? "true": "false");
}
#endif
