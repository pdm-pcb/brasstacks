#ifndef BRASSTACKS_UTILITY_MEMTRACKER_HPP
#define BRASSTACKS_UTILITY_MEMTRACKER_HPP

#include <cstdint>
#include <cstdio>
#include <cassert>
#include <list>

namespace btx {

static std::size_t bytes_allocated  = 0;
static std::size_t allocation_count = 0;
static std::size_t free_count       = 0;

} // namespace btx

#ifdef DEBUG

void * operator new(std::size_t bytes) {
    btx::bytes_allocated += bytes;
    std::printf(
        "%zu bytes allocated; %zu total, %zu / %zu\n",
        bytes, btx::bytes_allocated, ++btx::allocation_count,
        btx::free_count
    );

    void *new_alloc = malloc(bytes + sizeof(std::size_t));
    static_cast<std::size_t *>(new_alloc)[0] = bytes;
    return &(static_cast<std::size_t *>(new_alloc)[1]);
}

void operator delete(void *memory) {
    std::size_t bytes = static_cast<std::size_t *>(memory)[-1];
    btx::bytes_allocated -= bytes;
    std::printf(
        "%zu bytes freed; %zu total, %zu / %zu\n",
        bytes, btx::bytes_allocated, btx::allocation_count,
        ++btx::free_count
    );

    free(&(static_cast<std::size_t *>(memory)[-1]));
    if(btx::bytes_allocated == 0) {
        std::printf("Mission accomplished.\n");
    }
}

void * operator new[](std::size_t bytes) {
    btx::bytes_allocated += bytes;
    std::printf(
        "%zu bytes []allocated; %zu total, %zu / %zu\n",
        bytes, btx::bytes_allocated, ++btx::allocation_count,
        btx::free_count
    );

    void *new_alloc = malloc(bytes + sizeof(std::size_t));
    static_cast<std::size_t *>(new_alloc)[0] = bytes;
    return &(static_cast<std::size_t *>(new_alloc)[1]);
}

void operator delete[](void *memory) {
    std::size_t bytes = static_cast<std::size_t *>(memory)[-1];
    btx::bytes_allocated -= bytes;
    std::printf(
        "%zu bytes []freed; %zu total, %zu / %zu\n",
        bytes, btx::bytes_allocated, btx::allocation_count,
        ++btx::free_count
    );

    free(&(static_cast<std::size_t *>(memory)[-1]));
    if(btx::bytes_allocated == 0) {
        std::printf("Mission accomplished.\n");
    }
}

#endif // DEBUG

#endif // BRASSTACKS_UTILITY_MEMTRACKER_HPP