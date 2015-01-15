#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>

#define BASE_SIZE 2048

static void* (*real_malloc)(size_t) = NULL;
static void (*real_free)(void*) = NULL;
static void* (*real_realloc)(void* ptr, size_t size) = NULL;

struct linear_buffer_t {
    void *ptr;
    size_t allocated;
    size_t size;
};

static struct linear_buffer_t *memory_linear_buffer = NULL;

static struct linear_buffer_t* get_linear_buffer() {
    real_malloc = dlsym(RTLD_NEXT, "malloc");
    if (NULL == real_malloc) {
        fprintf(stderr, "dlsym for malloc error: %s\n", dlerror());
    }

    real_realloc = dlsym(RTLD_NEXT, "realloc");
    if (NULL == real_realloc) {
        fprintf(stderr, "dlsym for realloc error: %s\n", dlerror());
    }

    real_free = dlsym(RTLD_NEXT, "free");
    if (NULL == real_free) {
        fprintf(stderr, "dlsym for free error: %s\n", dlerror());
    }

    if (memory_linear_buffer == NULL) {
        memory_linear_buffer = real_malloc(sizeof(struct linear_buffer_t*));
        memory_linear_buffer->ptr = real_malloc(BASE_SIZE);
        memory_linear_buffer->size = BASE_SIZE;
        memory_linear_buffer->allocated = 0;
    } 

    return memory_linear_buffer;
}

static void expand_linear_buffer(size_t size) {
    struct linear_buffer_t *linear_buffer = get_linear_buffer();
    size_t expand_size = (size / BASE_SIZE + 1) * BASE_SIZE;

    printf("linear buffer expanded old = (base pointer = %p, size = %zu, allocated = %zu), ", 
        linear_buffer->ptr, linear_buffer->size, linear_buffer->allocated);

    linear_buffer->ptr = real_realloc(linear_buffer->ptr, linear_buffer->size + expand_size);
    linear_buffer->size += size;

    printf("new = (base pointer = %p, size = %zu, allocated = %zu)\n", 
        linear_buffer->ptr, linear_buffer->size, linear_buffer->allocated);
}

static void* alloc_in_liner_buffer(size_t size) {
    struct linear_buffer_t *linear_buffer = get_linear_buffer();
    void* ptr;

    if (linear_buffer == NULL) {
        return NULL;
    }

    if (linear_buffer->allocated + size > linear_buffer->size) {
        expand_linear_buffer(size);
    }

    linear_buffer->allocated += size;
    ptr = linear_buffer->ptr + linear_buffer->allocated;
    printf("linear buffer alloc(%zu) = %p (base pointer = %p, size = %zu, allocated = %zu)\n", size, ptr, 
        linear_buffer->ptr, linear_buffer->size, linear_buffer->allocated);
    return ptr;
}

void* malloc(size_t size) {
    printf("hook malloc(%zu) \n", size);
    return alloc_in_liner_buffer(size);
}

void* realloc(void* ptr, size_t size) {
    printf("hook realloc(%zu) \n", size);
    return alloc_in_liner_buffer(size);
}

void free(void *ptr) {
    struct linear_buffer_t *linear_buffer = get_linear_buffer();
    printf("hook free(%p) ignored (base pointer = %p, size = %zu, allocated = %zu)\n", ptr, 
        linear_buffer->ptr, linear_buffer->size, linear_buffer->allocated);
    ptr = NULL;
    return;
}

