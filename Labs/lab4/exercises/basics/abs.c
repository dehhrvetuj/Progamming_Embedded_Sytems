
#include <limits.h>
/*@
 requires INT_MIN < x <= INT_MAX;
 ensures \result >= 0;
 ensures \result == (x>=0? x:-x);
 assigns \nothing;
*/

int abs(int x)
{
    return (x >= 0 ? x : -x);
}

int v;

/*@ assigns v; */
void main(void)
{
    v = 10;
    int r1 = abs(-3);
    int r2 = abs(5);
    //@ assert r1 == 3;
    //@ assert r2 == 5;
    //@ assert v == 10;
}

