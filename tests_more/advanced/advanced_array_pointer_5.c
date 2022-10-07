int f(int a[10]){
    int *p;
    p = a;
    *(p+2) = 32;
    return p[2];
}
