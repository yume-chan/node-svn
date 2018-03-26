#include "malloc.hpp"

#include <stdio.h>

extern "C" {

std::atomic_int64_t memory_delta = 0;

#if defined(__GLIBC__)

#include <stddef.h>

extern void* __malloc(size_t size);
extern void* __realloc(void* block, size_t new_size);
extern void  __free(void* block);

void* malloc(size_t size) {
    void* result = __malloc(size);

    if (result != nullptr) {
        memory_delta += size;
    }

    return result;
}

void* realloc(void* block, size_t new_size) {
    auto size = malloc_usable_size(block);

    auto result = __realloc(block, new_size);

    if (result != nullptr) {
        memory_delta += new_size - size;
    }

    return result;
}

void free(void* block) {
    if (block == nullptr) {
        return;
    }

    auto size = malloc_usable_size(block);
    memory_delta -= size;

    __free(block);
}

#elif defined(WIN32)

#include <crtdbg.h>
#include <malloc.h>

void* malloc(size_t size) {
#ifdef _DEBUG

    auto result = _malloc_dbg(size, _NORMAL_BLOCK, nullptr, 0);

#else

    auto result = _malloc_base(size);

#endif

    if (result != nullptr) {
        memory_delta += size;
    }

    return result;
}

void* realloc(void* block, size_t new_size) {
    auto size = _msize(block);

#ifdef _DEBUG

    auto result = _realloc_dbg(block, new_size, _NORMAL_BLOCK, nullptr, 0);

#else

    auto result = _realloc_base(block, new_size);

#endif

    if (result != nullptr) {
        memory_delta += new_size - size;
    }

    return result;
}

void free(void* block) {
    if (block == nullptr) {
        return;
    }

    auto size = _msize(block);
    memory_delta -= size;

#ifdef _DEBUG

    _free_dbg(block, _NORMAL_BLOCK);

#else

    _free_base(block);

#endif
}
#endif
}
