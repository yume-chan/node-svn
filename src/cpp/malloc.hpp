#pragma once

#include <atomic>

extern std::atomic_int64_t memory_delta;

extern "C" {
#include <stddef.h>

void* malloc(size_t size);
void* realloc(void* block, size_t new_size);
void  free(void* block);
}
