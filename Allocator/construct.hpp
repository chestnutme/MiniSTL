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
inline void destroy(ForwardIterator first, ForwardIterator last) {
    __destroy_aux(first, last, has_trivial_destructor_t<value_type_t<ForwardIterator> >());
}

template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, true_type) {}

template <class ForwardIterator>
inline void __destroy_aux(ForwardIterator first, ForwardIterator last, false_type) {
    for(;first != last;first++) {
        destroy(&*first); // typeof(first) != T
    }
}

} // MiniSTL
