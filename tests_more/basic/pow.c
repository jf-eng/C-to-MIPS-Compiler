float f(float x, int n)
{
    float acc=1.0f;
    float test = 5.0f;
    int i=0;
    while(i<n){
        i++;
        acc=acc*x;
    }
    acc += test + 3.0f;
    return acc;
}
