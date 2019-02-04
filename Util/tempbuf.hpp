#pragma once

#include <cstdlib>
#include <cstddef>
#include <climits>
#include <exception>

#include "pair.hpp"
#include "Allocator/memory.hpp"
#include "Traits/type_traits.hpp"

namespace MiniSTL {

template <class T>
pair<T*, ptrdiff_t> 
__get_temporary_buffer(ptrdiff_t len, T*) {
    if(len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
        len = INT_MAX / sizeof(T);

    while(len > 0) {
        T* tmp = reinterpret_cast<T*>(malloc(static_cast<size_t>(len) * sizeof(T)));
        if(tmp)
            return pair<T*, ptrdiff_t>(tmp, len);
        len /= 2;
    }

    return pair<T*, ptrdiff_t>(reinterpret_cast<T*>(nullptr), 0);
}

template <class T>
inline pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t len) {
    return __get_temporary_buffer(len, reinterpret_cast<T*>(nullptr));
}


template <class T>
void return_temporary_buffer(T* p) {
    free(p);
}

template <class ForwardIt, class T>
class Temporary_Buffer {
private:
    ptrdiff_t original_len;
    ptrdiff_t len;
    T* buffer;

    void allocate_buffer() {
        original_len = len;
        buffer = 0;

        if(len > static_cast<ptrdiff_t>(INT_MAX / sizeof(T)))
            len = INT_MAX / sizeof(T);

        while(len > 0) {
            buffer = reinterpret_cast<T*>(malloc(len * sizeof(T)));
            if(buffer)
                break;
            len /= 2;
        }
    }

    void initialize_buffer(const T&, true_type) {}
    void initialize_buffer(const T& val, false_type) {
        uninitialized_fill_n(buffer, len, val);
    }

public:
    ptrdiff_t size() const { return len; }
    ptrdiff_t requested_size() const { return original_len; }
    
    T* begin() { return buffer; }
    T* end() { return buffer + len; }

    Temporary_Buffer(ForwardIt first, ForwardIt last) {
        try {
            len = 0;
            distance(first, last, len);
            allocate_buffer();
            if(len > 0)
                initialize_buffer(*first, is_POD_type_t<ForwardIt>());
        } catch(std::exception&) {
            free(buffer); 
            buffer = nullptr;
            len = 0;
        }
    }
    
    ~Temporary_Buffer() {  
        destroy(buffer, buffer + len);
        free(buffer);
    }

public:
    // disable copy ctor and copy assign
    Temporary_Buffer(const Temporary_Buffer&) = delete;
    Temporary_Buffer& operator=(const Temporary_Buffer&) = delete;
};

} // MiniSTL
