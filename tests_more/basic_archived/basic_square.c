int square1() {
    int a = 5;
    a = 1 + 10;
    return a;
}

int square2() {
    int b = 5;
    b = 1 + 10;
    {
        b = 13;
    }
    return b;
}