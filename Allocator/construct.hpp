#pragma once

#include <new>
#include "../Traits/typeTraits.hpp"

namespace MiniSTL {

template <class T1, class T2>
inline void construct(T1* p, T2 v) {
    new(p) T1(v);
}

template <class T>
inline void destroy(T* p) {
    p->~T();
}

template <class ForwardIterator>
inline void destory(ForwardIterator first, ForwardIterator last) {
    __destory(first, last, value_type(first));
}

template <class ForwardIterator, class T>
inline void __destory(ForwardIterator first, ForwardIterator last, T*) {
    using has_trivial_destructor = typename type_traits<T>::has_trivial_destructor;
    __destory_aux(first, last, has_trivial_destructor());
}

template <class ForwardIterator, class T>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, true_type) {}

template <class ForwardIterator, class T>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, false_type) {
    for(;first != last;first++) {
        destory(&*first); // typeof(first) != T
    }
}

};
