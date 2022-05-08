
#include "pch.h"
#include <Windows.h>
#include "taskflow.impl.h"

namespace taskflow { namespace dotnet { namespace impl { 

    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    static unsigned char _default_alloc_mem[sizeof(default_allocator)];
    static default_allocator* _default_alloc_instance = new(_default_alloc_mem) default_allocator();
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    mem_allocator::ptraddr_t default_allocator::allocate(std::size_t size) {
        mem_allocator::ptraddr_t ptr = ::operator new(size);
        return ptr;
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void default_allocator::deallocate(mem_allocator::ptraddr_t ptr) {
        ::operator delete(ptr);
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    const std::shared_ptr<mem_allocator> & default_allocator::instance() {
        static std::shared_ptr<mem_allocator> p{ _default_alloc_instance };
        return p;
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    win32_heapalloc_allocator::win32_heapalloc_allocator(std::size_t initialSize, std::size_t maxSize) {
        // HEAP_NO_SERIALIZE
        _hHeap = create_heap(0, initialSize, maxSize);
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    win32_heapalloc_allocator::heap_ptr create_heap(std::size_t flags, std::size_t initialSize, std::size_t maxSize) {
        return HeapCreate(flags, initialSize, maxSize);
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    win32_heapalloc_allocator::~win32_heapalloc_allocator() {
        HeapDestroy(_hHeap);
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    mem_allocator::ptraddr_t win32_heapalloc_allocator::allocate(std::size_t size) {
        mem_allocator::ptraddr_t ptr = HeapAlloc(_hHeap, HEAP_ZERO_MEMORY, size);
        return ptr;
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    void win32_heapalloc_allocator::deallocate(mem_allocator::ptraddr_t ptr) {
        HeapFree(_hHeap, 0, ptr);
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

} } }