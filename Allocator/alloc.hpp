#pragma once

#include <new>
#include <cstdlib>
#include <cstring>


namespace MiniSTL {

// malloc-based allocator
class malloc_alloc {

    using malloc_handler_t = void (*)();

private:

    static void* oom_malloc(size_t);
    static void* oom_realloc(void*, size_t);
    static malloc_handler_t malloc_alloc_oom_handler;

public:

    static void* allocate(size_t sz) {
        void* res = std::malloc(sz);
        if(res == nullptr) {
            res = oom_malloc(sz);
        }
        return res;
    }

    static void deallocate(void* p, size_t /* sz */) {
        free(p);
    }

    static void* realloc(void* p, size_t /* old_sz */, size_t new_sz) {
        void* res = std::realloc(p, new_sz);
        if(res == nullptr) {
            res = oom_realloc(p, new_sz);
        }
        return res;
    }

    static malloc_handler_t set_malloc_handler(malloc_handler_t f) {
        malloc_handler_t old = malloc_alloc_oom_handler;
        malloc_alloc_oom_handler = f;
        return old;
    }
};

typename malloc_alloc::malloc_handler_t malloc_alloc::malloc_alloc_oom_handler = nullptr;

void* malloc_alloc::oom_malloc(size_t sz) {
    malloc_handler_t my_malloc_handler;
    void* res;
    for(;;) {
        my_malloc_handler = malloc_alloc_oom_handler;
        if(my_malloc_handler == nullptr) {
            throw std::bad_alloc();
        }
        malloc_alloc_oom_handler();
        res = std::malloc(sz);
        if(res != 0) {
            return res;
        }
    }
}

void* malloc_alloc::oom_realloc(void* p, size_t sz) {
    malloc_handler_t my_malloc_handler;
    void* res;
    for(;;) {
        my_malloc_handler = malloc_alloc_oom_handler;
        if(my_malloc_handler == nullptr) {
            throw std::bad_alloc();
        }
        malloc_alloc_oom_handler();
        res = std::realloc(p, sz);
        if(res != 0) {
            return res;
        }
    }
}

// Comparison operators for all of the predifined SGI-style allocators.
// This ensures that allocator<malloc_alloc> (for example) will
// work correctly.
inline bool operator==(const malloc_alloc&,
                       const malloc_alloc&)
{
  return true;
}

template
inline bool operator!=(const malloc_alloc&,
                       const malloc_alloc&)
{
  return false;
}

// Default allocator
// Implementation properties:
//      1. If request an object of size > MAX_BYTES, then result object
//      will be obtained directly from malloc alloc
//      2. In all other cases, it allocates an object of size exactly
//      roundUp(size).Then we can return the object to the proper free list
//      due to size info without permanentyly losing part of the object.
class default_alloc {
private:
    enum  freelist_setting{
	    ALIGN = 8,
        MAX_BYTES = 128, 
        NFREELISTS = MAX_BYTES / ALIGN
    };

    static size_t roundUp(size_t bytes) {
        return (((bytes) + static_cast<size_t>(ALIGN) - 1) 
                & ~(static_cast<size_t>(ALIGN) - 1));
    }

    union obj {
        union obj* freelist_link;
        char data[1];
    };

    static obj* volatile freelist[NFREELISTS];

    static size_t freelist_index(size_t bytes) {
        return (((bytes) + static_cast<size_t>(ALIGN) - 1) / 
                static_cast<size_t>(ALIGN) - 1);
    }

    // return an obj of size sz, and optionally adds to size sz freelist
    static void* refill(size_t sz);

    // Allocates a chunk for nobjs of size sz. nobjs may be reduced
    // if it can not allocate the requested number.
    static char* chunk_alloc(size_t sz, int& nobjs);

    // chunk allocation state
    static char* start_free;
    static char* end_free;
    static size_t heap_size;

public:

    static void* allocate(size_t sz);

    static void deallocate(void* p, size_t sz);

    static void* reallocate(void* p, size_t old_sz, size_t new_sz);
};

char* default_alloc::start_free = nullptr;
char* default_alloc::end_free = nullptr;
size_t heap_size = 0;
default_alloc::obj* volatile 
default_alloc::freelist[NFREELISTS] = {
    nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr,
    nullptr, nullptr, nullptr, nullptr,
};

char* default_alloc::chunk_alloc(size_t sz, int& nobjs) {
    char* res = nullptr;
    size_t total_bytes = sz * nobjs;
    size_t bytes_left = end_free - start_free;

    if(bytes_left >= total_bytes) {
        //case1: enough space for sz objs
        res = start_free;
        start_free += total_bytes;
        return res;
    } else if(total_bytes >= sz) {
        //case2: enough space for k(k<sz) objs
        nobjs = static_cast<int>(total_bytes / sz);
        res = start_free;
        start_free += sz * nobjs;
        return res;
    } else {
        // case3: no enough space for just one obj
        size_t bytes_to_get = 2 * total_bytes + roundUp(heap_size>>4);
        if(bytes_left > 0) {
            //make use of left-over piece by add it into smaller freelist
            obj* volatile* my_freelist = freelist + freelist_index(bytes_left);
            reinterpret_cast<obj*>(start_free)->freelist_link = *my_freelist;
            *my_freelist = reinterpret_cast<obj*>(start_free);
        }
        start_free = reinterpret_cast<char*>(std::malloc(bytes_to_get));
        if(start_free == nullptr) {
            // not enough space in heap
            obj* volatile* my_freelist;
            obj* o;
            // find space in bigger freelist
            for(size_t i = sz; i <= static_cast<size_t>(MAX_BYTES);i += static_cast<size_t>(ALIGN)) {
                my_freelist = freelist + freelist_index(i);
                o = *my_freelist;
                if(o) {
                    // find and get the block, then recursive call to revise and adopt, which will be case1 or case2
                    *my_freelist = o->freelist_link;
                    start_free = reinterpret_cast<char*>(o);
                    end_free = start_free + i;
                    return chunk_alloc(sz, nobjs);
                }
            }
            // no memory everywhere
            end_free = nullptr;
            // call malloc_alloc, then get enough space or throw an exception
            start_free = reinterpret_cast<char*>(malloc_alloc::allocate(bytes_to_get));
        }
        // we get enough space
        heap_size += bytes_to_get;
        end_free = start_free + bytes_to_get;
        // recursive call to revise and adopt
        return chunk_alloc(sz, nobjs);
    }
} 

// get sz size space and build freelist by call chunk_alloc
// sz must be aligned
void* default_alloc::refill(size_t sz) {
    int nobjs = 20;
    char* chunk = chunk_alloc(sz, nobjs);
    obj* volatile* my_freelist;
    obj *cur_obj, *next_obj;

    if(nobjs == 1)
        return reinterpret_cast<void*>(chunk);
    
    // build freelist by chaining sized-sz blocks from chunk
    my_freelist = freelist + freelist_index(sz);
    *my_freelist = cur_obj = reinterpret_cast<obj*>(chunk + sz);
    for(int i = 1; i < nobjs;i++) {
        next_obj = reinterpret_cast<obj*>(reinterpret_cast<char*>(cur_obj) + sz);
        if(i == nobjs - 1) {
            cur_obj->freelist_link = nullptr;
        } else {
            cur_obj->freelist_link = next_obj;
        }
        cur_obj = next_obj;
    }
    return reinterpret_cast<void*>(reinterpret_cast<obj*>(chunk));
}

void* default_alloc::allocate(size_t sz) {
    void* res = nullptr;
    if(sz > static_cast<size_t>(MAX_BYTES)) {
        res = malloc_alloc::allocate(sz);
    } else {
        obj* volatile* my_freelist = freelist + freelist_index(sz);
        obj* o = *my_freelist;
        if(o == nullptr) {
            res = refill(sz);
        } else {
            *my_freelist = o->freelist_link;
            res = reinterpret_cast<void*>(o);
        }
    }
    return res;
}

void default_alloc::deallocate(void* p, size_t sz) {
    if(sz > static_cast<size_t>(MAX_BYTES)) {
        malloc_alloc::deallocate(p, sz);
    } else {
        obj* volatile* my_freelist = freelist + freelist_index(sz);
        obj* o = reinterpret_cast<obj*>(p);
        o->freelist_link = *my_freelist;
        *my_freelist = o;
    }
}

void* default_alloc::reallocate(void* p, size_t old_sz, size_t new_sz) {
    void* res;
    if(old_sz > static_cast<size_t>(MAX_BYTES) && new_sz > static_cast<size_t>(MAX_BYTES)) {
        return std::realloc(p, new_sz);
    }
    if(roundUp(old_sz) == roundUp(new_sz)) {
        return p;
    }
    res = allocate(new_sz);
    memcpy(res, p, new_sz > old_sz ? old_sz : new_sz);
    deallocate(p, old_sz);
    return res;
}

} // MiniSTL
