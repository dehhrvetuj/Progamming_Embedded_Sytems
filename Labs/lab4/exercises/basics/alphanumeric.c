/*@
  assigns \nothing;
  ensures \result <==>
    (('0' <= c && c <= '9') ||
     ('A' <= c && c <= 'Z') ||
     ('a' <= c && c <= 'z'));
*/
int is_alphanumeric(char c)
{
    if ('0' <= c && c <= '9') {
        return 1;
    }

    if ('A' <= c && c <= 'Z') {
        return 1;
    }

    if ('a' <= c && c <= 'z') {
        return 1;
    }

    return 0;

}

void main()
{
    int r;

    r = is_alphanumeric('0');
    //@ assert r;

    r = is_alphanumeric('x');
    //@ assert r;

    r = is_alphanumeric(' ');
    //@ assert !r;
}
