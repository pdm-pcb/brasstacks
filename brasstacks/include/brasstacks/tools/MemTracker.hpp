#ifndef BRASSTACKS_TOOLS_MEMTRACKER_HPP
#define BRASSTACKS_TOOLS_MEMTRACKER_HPP

#include "brasstacks/pch.hpp"

// Keeping tabs on news and deletes
struct MemTracker final {
    static size_t total_bytes;
    static size_t alloc_count;
    static size_t free_count;
};

// Wrapping native memory management to facilitate the above tabs-keeping
#ifdef BTX_DEBUG
    void * operator new(size_t bytes);
    void * operator new[](size_t bytes);
    void * operator new(size_t size, const std::nothrow_t&) noexcept;

    void operator delete(void *memory)  noexcept;
    void operator delete[](void *memory) noexcept;
    void operator delete(void *memory, size_t)  noexcept;
    void operator delete[](void *memory, size_t) noexcept;
    void operator delete(void *memory, const std::nothrow_t&) noexcept;
#endif // BTX_DEBUG

#endif // BRASSTACKS_TOOLS_MEMTRACKER_HPP