#include <limits.h>

/*@
@ requires \valid(a) && \valid(b);
@ requires INT_MIN <= *a <= INT_MAX;
@ requires INT_MIN <= *b <= INT_MAX;
@ requires INT_MIN - *b <= *a <= INT_MAX - *b;
@ requires INT_MIN - *a <= *b <= INT_MAX - *a;
@ assigns \nothing;
@ ensures \result == *a + *b;
@ 
@ */

int add_ptr(int* a, int* b)
{
    return (*a + *b);
}

//@ assigns \nothing;
void main(void)
{
    int a = 13;
    int b = 15;
    int r = add_ptr(&a, &b);
    //@ assert r == 28;
}
