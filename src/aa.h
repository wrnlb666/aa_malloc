#ifndef __AA_H__
#define __AA_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <threads.h>


#ifdef __linux__ 
    #include <sys/mman.h>
#elif defined _WIN32
    #include <Windows.h>
#endif  // __linux__



void*   aa_malloc(size_t size);
void*   aa_realloc(void* ptr, size_t size);
void*   aa_calloc(size_t nmemb, size_t size);
void    aa_free(void);                          // call aa_free to reuse the memory
void    aa_destroy(void);                       // call aa_destroy when exiting from a thread including main thread



#endif  // __AA_H__
