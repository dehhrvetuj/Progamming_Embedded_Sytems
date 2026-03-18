#include <limits.h>
/*@
 requires INT_MIN <= x <= INT_MAX;
 requires INT_MIN <= y <= INT_MAX;
 ensures \result >= x;
 ensures \result >= y;
 ensures x >= y ==> \result == x;
 ensures x < y  ==> \result == y;
 assigns \nothing;
*/

int max(int x, int y)
{
    return (x >= y ? x : y);
}


int v;

/*@ assigns v; */
void main(void)
{
    v = 10;
    int r1 = max(1,3);
    int r2 = max(-1,-5);
    int r3 = max(-5,0);
    //@ assert r1 == 3;
    //@ assert r2 == -1;
    //@ assert r3 == 0;
    //@ assert v == 10;
}