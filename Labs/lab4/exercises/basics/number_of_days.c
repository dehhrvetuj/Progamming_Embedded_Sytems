/*@
 requires 1 <= month <= 12;
 ensures \result == 28 || \result == 30 || \result == 31;
 ensures (month == 2)  <==> (\result == 28);
 ensures (month == 4 || month == 6 || month == 9 || month == 11) <==> (\result == 30);
 ensures (month == 1 || month == 3 || month == 5 || month == 7 || month == 8  || month == 10 || month == 12) <==> (\result == 31);
 assigns \nothing;

@ */

int number_of_days(int month){
  int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  return days[month-1];
}


int v;

/*@ assigns v; */
void main(void)
{
    v = 10;
    int r1 = number_of_days(2);
    int r2 = number_of_days(3);
    int r3 = number_of_days(4);
    //@ assert r1 == 28;
    //@ assert r2 == 31;
    //@ assert r3 == 30;
    //@ assert v == 10;
}