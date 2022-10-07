int f(){
    int a[5] = {1,2,3,4,5};
    int *p = a + 1;
    ++p;
    --p;
    return *p;
}