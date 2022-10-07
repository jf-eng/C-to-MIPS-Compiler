int f(int a[10]);

int main ()
{
  int a[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
  return (!f(a)==32);
}