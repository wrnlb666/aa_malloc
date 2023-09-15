#include "aa.h"
#include <pthread.h>
#include <stdlib.h>


// single allocation node, fragmentation
typedef struct aa_frag aa_frag_t;
struct aa_frag {
    int64_t     magic_num;
    uint64_t    size;
    char        data[];
};

// define magic_num for frag
#define MAGIC_NUM 114514

// aa_arena single region
typedef struct aa_region aa_region_t;
struct aa_region {
    aa_region_t*    next;
    uint64_t        capacity;
    uint64_t        cursor;
    uintptr_t       data[];
};

// memory pool, one for each thread
typedef struct aa_arena aa_arena_t;
struct aa_arena {
    aa_region_t*    head;
    aa_region_t*    tail;
};
thread_local aa_arena_t aa_arena = { 0 };


// default capacity (8 * 1024) in 8 bytes, would be 2 pages for most arch
#define DEFAULT_CAP (1024)

// define page size, which is normally 4kb
#define PAGE_SIZE (4 * 1024)


static inline aa_region_t* aa_region_init(size_t capacity);
static inline void aa_region_destroy(aa_region_t* region);


static inline aa_region_t* aa_region_init(size_t capacity) {
    uint64_t size = sizeof (aa_region_t) + DEFAULT_CAP * sizeof (uintptr_t);
    
    // page align
    void* ptr = NULL;
    #ifdef __linux__
        if (posix_memalign(&ptr, PAGE_SIZE, size) != 0 ) {
            fprintf(stderr, "[ERRO]: OOM\n");
            exit(1);
        }
    #elif defined WIN32
        ptr = _aligned_malloc(size, PAGE_SIZE);
    #endif  // __linux__

    if (ptr == NULL) {
        fprintf(stderr, "[ERRO]: OOM\n");
        exit(1);
    }
    aa_region_t* region = ptr;

    *region = (aa_region_t) {
        .next       = NULL, 
        .capacity   = capacity, 
        .cursor     = 0, 
    };
    return region;
}

static inline void aa_region_destroy(aa_region_t* region) {
    #ifdef __linux__
        free(region);
    #elif defined WIN32
        _aligned_free(region)
    #endif  // __linux__
}


void* aa_malloc(size_t size) {
    // mem align
    size_t size_8bytes = size + (sizeof (uintptr_t) - 1) / sizeof (uintptr_t);
    size = size_8bytes + sizeof (aa_frag_t);

    if (aa_arena.tail == NULL) {
        if (aa_arena.head != NULL) {
            fprintf(stderr, "[ERRO]: aa internal linked list corrupted\n");
            exit(1);
        }
        size_t cap = DEFAULT_CAP;
        if (cap < size) cap = size;
        aa_arena.tail = aa_region_init(cap);
        aa_arena.head = aa_arena.tail;
    }

    while ((aa_arena.tail->cursor + size > aa_arena.tail->capacity) && (aa_arena.tail->next != NULL)) {
        aa_arena.tail = aa_arena.tail->next;
    }

    if (aa_arena.tail->cursor + size > aa_arena.tail->capacity) {
        if (aa_arena.tail->next != NULL) {
            fprintf(stderr, "[ERRO]: aa internal linked list corrupted\n");
            exit(1);
        }
        size_t capacity = DEFAULT_CAP;
        if (capacity < size) capacity = size;
        aa_arena.tail->next = aa_region_init(capacity);
        aa_arena.tail = aa_arena.tail->next;
    }

    aa_frag_t* result = (void*) &aa_arena.tail->data[aa_arena.tail->cursor];
    result->size = size_8bytes;
    result->magic_num = MAGIC_NUM;
    aa_arena.tail->cursor += size;
    return &result->data;
}


void* aa_realloc(void *ptr, size_t size)
{
    aa_frag_t* prev = ptr - offsetof(aa_frag_t, data);
    if (prev->magic_num != MAGIC_NUM) {
        fprintf(stderr, "[ERRO]: 0X%p is not allocated(via aa)\n", ptr);
        exit(1);
    }

    size_t old_size = prev->size;
    if (size <= old_size) return ptr;

    if ((char*) ptr + old_size == (char*) &aa_arena.tail->data[aa_arena.tail->cursor]) {
        size_t temp = ((size - old_size) + sizeof(uintptr_t) - 1)/sizeof(uintptr_t);
        if (aa_arena.tail->cursor + temp <= aa_arena.tail->capacity) {
            aa_arena.tail->cursor += temp;
            return ptr;
        }
    }

    void *new_ptr = aa_malloc(size);
    memcpy(new_ptr, ptr, old_size);
    return new_ptr;
}


void* aa_calloc(size_t nmemb, size_t size) {
    size = nmemb * size;
    void* res = aa_malloc(size);
    memset(res, 0, size);
    return res;
}


void aa_free(void) {
    for (aa_region_t* temp = aa_arena.head; temp != NULL; temp = temp->next) {
        temp->cursor = 0;
    }
    aa_arena.tail = aa_arena.head;
}


void aa_destroy(void) {
    aa_region_t* region = aa_arena.head;
    while (region != NULL) {
        aa_region_t* temp = region;
        region = region->next;
        aa_region_destroy(temp);
    }
    aa_arena.head = NULL;
    aa_arena.tail = NULL;
}


// arena initialization
__attribute__((constructor))
static void aa_arena_init(void) {
    atexit(aa_destroy);
}

