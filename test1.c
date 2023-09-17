#include "src/aa.h"


int main(void) {
    void* foo1 = aa_malloc(300);
    printf("foo1: 0X%p\n", foo1);
    void* foo2 = aa_malloc(300);
    printf("foo2: 0X%p\n", foo2);
    foo2 = aa_realloc(foo2, 500);
    printf("foo2: 0X%p\n", foo2);
    foo1 = aa_realloc(foo1, 500);
    printf("foo1: 0X%p\n", foo1);


    
    aa_destroy();
    return 0;
}
