int f(){
    int i;
    int k = 0;
    for(i = 0; i < 10; i++){
        if(i < 5) continue;
        k++;
    }
    return k;
}