int f(int a[10])
{
  *(a+2) = 32;
  int *p = &a + 2;
  return *p;
}
