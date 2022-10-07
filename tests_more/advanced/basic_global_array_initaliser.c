int x[5] = {1,2,3,4,5};

int f()
{
    int i;
    for(i=0; i<4; i++){
        x[i]=i;
    }
    return x[4];
}
