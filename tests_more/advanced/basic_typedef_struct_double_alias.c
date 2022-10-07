typedef struct S {
    int a;
    int b;
} x;

int f(){
    x y;
    y.a = 14;
    y.b = 12;
    return y.a;
}