/*@
    requires n >= 1;
    requires \valid_read(a + (0 .. n-1));
    requires \valid_read(b + (0 .. n-1));

    assigns \nothing;

    behavior equal:
        assumes \forall int i; 0 <= i < n ==> a[i] == b[i];
        ensures \result == 1;
    
    behavior not_equal:
        assumes \exists int i; 0 <= i < n && a[i] != b[i];
        ensures \result == 0;
    
    complete behaviors;
    disjoint behaviors;
*/
int array_equals(int* a, int* b, int n)
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
            return 0;
        }
    }
    
    return 1;
}

int main(void)
{
    int a[] = {3, 5, 18, 2, 12};
    int b[] = {3, 5, 18, 4, 19};
    int r1 = array_equals(a, b, 3);
    int r2 = array_equals(a, b, 4);
    //@ assert r1 == 1;
    //@ assert r2 == 0;
    return 0;
}
