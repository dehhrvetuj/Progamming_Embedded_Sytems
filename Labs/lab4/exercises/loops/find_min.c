#include <limits.h>

/*@
  requires 0 < len < INT_MAX;
  requires \valid(arr + (0 .. len-1));
  assigns \nothing;
  ensures \forall int i; 0 <= i < len ==> \result <= arr[i];
  ensures \exists int i; 0 <= i < len && \result == arr[i];
*/
int find_min(int* arr, int len)
{
    int min_idx = 0;

    /*@
      loop invariant 1 <= i <= len;
      loop invariant 0 <= min_idx < i;
      loop invariant \forall int j; 0 <= j < i ==> arr[min_idx] <= arr[j];
      loop assigns i, min_idx;
      loop variant len - i;
    */
    for (int i = 1; i < len; i++) {
        if (arr[i] < arr[min_idx]) {
            min_idx = i;
        }
    }

    return arr[min_idx];
}

int main(void)
{
    int a[] = {3, 5, 18, 2, 12};
    int r = find_min(a, 5);
    //@ assert r == 2;
    return 0;
}