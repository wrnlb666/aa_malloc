#include "src/aa.h"

#include <stdio.h>

int main(void) {
    void* foo = aa_malloc(300);
    printf("foo: 0X%p\n", foo);
    foo = aa_malloc(500);
    printf("foo: 0X%p\n", foo);

    aa_free();
    foo = aa_malloc(300);
    printf("foo: 0X%p\n", foo);
    foo = aa_malloc(500);
    printf("foo: 0X%p\n", foo);

    aa_destroy();
    return 0;
}
