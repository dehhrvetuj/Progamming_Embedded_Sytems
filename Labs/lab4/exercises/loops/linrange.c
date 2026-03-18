#include <limits.h>

/*@
 requires 0 < len <= INT_MAX;
 requires INT_MIN <= start <= INT_MAX - len;
 requires \valid(arr + (0 .. len-1));
 assigns arr[0 .. len-1];
 ensures \forall int i; 0 <= i < len ==> arr[i] == start + i;

*/
void linrange(int* arr, int len, int start)
{
    if (!arr)
        return;
    
    /*@
        loop invariant 0 <= i <= len;
        loop invariant \forall int j; 0 <= j < i ==> arr[j] == j + start;
        loop assigns i, arr[0 .. len-1];
        loop variant len - i;
    */
    for (int i = 0; i < len; i++)
    {
        arr[i] = start + i;
    }
}

//@ assigns \nothing;
void main(void)
{
    int a[5];
    linrange(a, 5, 2);
    //@ assert a[0] == 2; 
    //@ assert a[1] == 3; 
    //@ assert a[2] == 4; 
    //@ assert a[3] == 5; 
    //@ assert a[4] == 6;
}
