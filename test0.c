#include "src/aa.h"

#include <stdio.h>
#include <pthread.h>


void* alloc_print(void* ptr) {
    (void) ptr;

    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            printf("allocated %d bytes at 0X%p\n", 300, aa_malloc(300));
        }
        aa_free();
    }

    aa_destroy();
    return NULL;
}

int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, alloc_print, NULL);
    alloc_print(NULL);
    pthread_join(tid, NULL);

    aa_destroy();
    return 0;
}
