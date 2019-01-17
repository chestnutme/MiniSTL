#pragma once

#include <new>
#include "Traits/type_traits.hpp"
#include "Iterator/iterator_base.hpp"

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
    __destory_aux(first, last, has_trivial_destructor<value_type<ForwardIterator> >());
}

template <class ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, true_type) {}

template <class ForwardIterator>
inline void __destory_aux(ForwardIterator first, ForwardIterator last, false_type) {
    for(;first != last;first++) {
        destory(&*first); // typeof(first) != T
    }
}

} // MiniSTL
