#include <limits.h>

/*@
@ requires \valid(x) && \valid(y);
@ requires \separated(x, y);
@ requires INT_MIN <= *x <= INT_MAX;
@ requires INT_MIN <= *y <= INT_MAX;
@ requires INT_MIN + *y <= *x <= INT_MAX + *y;
@ assigns *x;
@ ensures *x == \old(*x) - *y;
@ 
@ */
void decr_by(int* x, int const* y)
{
    *x = *x - *y;
}


void main(void)
{
    int x = 7;
    int y = 3;
    decr_by(&x, &y);
    //@ assert x == 4;
    //@ assert y == 3;
}
