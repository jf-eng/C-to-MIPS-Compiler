int f(int *a);

int main ()
{
  int a[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  f (a);
  return (!(a[2]==32));
}