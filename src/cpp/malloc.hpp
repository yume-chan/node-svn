#pragma once

#include <atomic>

extern "C" {

extern std::atomic_int64_t memory_delta;

void* malloc(size_t size);
void* realloc(void* block, size_t new_size);
void  free(void* block);
}
