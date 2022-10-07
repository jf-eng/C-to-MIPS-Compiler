struct x{
    int y;
    int* z;
};

int f()
{
    struct x g;
    g.y=17;
    g.z=&(g.y);
    *(g.z) = 27;
    return g.y;
}