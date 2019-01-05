#pragma once

#include <new>
#include <cstdlib>
#include <cstdint>
#include <cstddef>

namespace Unused {

template <class T>
inline T* allocate(ptrdiff_t n, T*) {
    set_new_handler(0);
    T* res = static_cast<T*>(::operator new(static_cast<size_t>(n * sizeof(T)));
    if (res == nullptr) {
        std::cout << "out of memory" << std::endl;
        exit(-1);
    }
    return res;
}

template <class T>
inline void deallocate(T* p) {
    ::operator delete(p);
}

template <class T1, class T2>
inline void construct(T1* p, const T2& v) {
  new(p) T1(v);
}

template <class T>
inline void construct(T* p) {
  new(p) T();
}

template <class T>
inline void destroy(T* p) {
  p->~T();
}

template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
  for(;first != last;first++){
      destroy(&*first);
  }
}


template <class T>
class allocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    template <class U>
    struct rebind {
        using other = allocator<U>;
    }

    pointer allocate(size_type sz, const void* hint = nullptr) {
        return ::allocate(static_cast<difference_type>(sz), nullptr);
    }

    void deallocate(poiner p, size_type sz) {
        ::deallocate(p);
    }

    void construst(pointer p, const_reference v) {
        ::construct(p, v);
    }

    void destroy(pointer p) {
        ::destroy(p);
    }

    pointer address(reference x) {
        return static_cast<pointer>(&x);
    }

    const_pointer const_address(reference x) {
        return static_cast<const_pointer>(&x);
    }

    size_type max_size() const {
        return static_cast<size_type>(SIZE_MAX / sizeof(T));
    }
};

}