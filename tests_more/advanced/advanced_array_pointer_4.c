int f(int *a){
    int *p = a;
    p[2] = 32;
    return *(p+2);
}
