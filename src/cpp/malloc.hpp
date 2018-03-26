#pragma once

#include <atomic>

template <class T>
struct atomic_counter {
  public:
    void operator+=(T value) {
        _value.fetch_add(value, std::memory_order_relaxed);
    }

    void operator-=(T value) {
        _value.fetch_sub(value, std::memory_order_relaxed);
    }

    T reset() {
        return _value.exchange(0, std::memory_order_relaxed);
    }

  private:
    std::atomic<T> _value;
};

extern atomic_counter<int64_t> memory_delta;

extern "C" {
#include <stddef.h>

void* malloc(size_t size);
void* realloc(void* block, size_t new_size);
void  free(void* block);
}
