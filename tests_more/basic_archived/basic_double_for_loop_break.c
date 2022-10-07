int f(){
    int i;
    int j;
    int acc = 0;
    for ( i = 0; i < 10; i++){
        for( j = 0; j < 10; j++){
            acc++;
            if( j == 5 ) break;
        }
        if( i == 5 ) break; 
    }
    return acc;
}