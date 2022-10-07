struct Book {
    int pages;
    int i;
};

int f(){
    struct Book b;
    b.pages = 1;
    return b.pages;
}