#ifndef __AA_H__
#define __AA_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <threads.h>
#include <pthread.h>



void*   aa_malloc(size_t size);
void*   aa_realloc(void* ptr, size_t size);
void*   aa_calloc(size_t nmemb, size_t size);
void    aa_free(void);
void    aa_destroy(void);



#endif  // __AA_H__
