/*@
    requires n >= 1;
    requires \valid_read(a + (0 .. n-1));
    requires \valid_read(b + (0 .. n-1));

    assigns \nothing;

    behavior match:
        assumes \forall int i; 0 <= i < n ==> a[i] == b[i];
        ensures \result == n;
    
    behavior mismatch:
        assumes \exists int i; 0 <= i < n && a[i] != b[i];
        ensures 0 <= \result < n;
        ensures a[\result] != b[\result];
        ensures \forall int j; 0 <= j < \result ==> a[j] == b[j];
    
    complete behaviors;
    disjoint behaviors;
*/
int mismatch(int* a, int* b, int n)
{
    /*@
      loop invariant 0 <= i <= n;
      loop invariant  \forall int j; 0 <= j < i ==> a[j] == b[j];
      loop assigns i;
      loop variant n - i;
    */
    for (int i = 0; i < n; i++)
    {
        if (a[i] != b[i])
        {
            return i;
        }
    }
    
    return n;
}


int main(void)
{
    int a[] = {3, 5, 18, 2, 12};
    int b[] = {3, 5, 18, 4, 19};
    int r1 = mismatch(a, b, 2);
    int r2 = mismatch(a, b, 4);
    //@ assert r1 == 2;
    //@ assert r2 == 3;
    return 0;
}