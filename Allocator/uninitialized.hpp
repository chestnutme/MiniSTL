#pragma once

#include <cstring>
#include "construct.hpp"
#include "Traits/type_traits.hpp"
#include "Iterator/iterator_base.hpp"
#include "Algorithms/algobase.hpp"
#include "Util/pair.hpp"

namespace MiniSTL {

// copy [first, last) into [res, res + last - first)
template <class InputIter, class ForwardIter>
inline ForwardIter uninitialized_copy(InputIter first, InputIter last, ForwardIter res) {
    return __uninitialized_copy_aux(first, last, res, is_POD_type<value_type<ForwardIter> >());
}

template <class InputIter, class ForwardIter>
inline ForwardIter __uninitialized_copy_aux(InputIter first, InputIter last, ForwardIter res, false_type) {
    ForwardIter cur = res;
    for(;first != last;++first, ++cur) {
        construct(&*cur, *first);
    }
    return cur;
}

template <class InputIter, class ForwardIter>
inline ForwardIter __uninitialized_copy_aux(InputIter first, InputIter last, ForwardIter res, true_type) {
    return copy(first, last, res);
}

inline char* uninitialized_copy(const char* first, const char* last, char* res) {
    memmove(res, first, last - first);
    return res + (last - first);
}

inline wchar_t* uninitialized_copy(const wchar_t* first, const wchar_t* last, wchar_t* res) {
    memmove(res, first, last - first);
    return res + (last - first);
}

// copy [first, first + n) into [res, res+n)
template <class InputIter, class Size, class ForwardIter>
inline pair<InputIter, ForwardIter> 
uninitialized_copy_n(InputIter first, Size n, ForwardIter res) {
    return __uninitialized_copy_n_aux(first, n, res, 
                iterator_category<ForwardIter>());
}

template <class InputIter, class Size, class ForwardIter>
inline pair<InputIter, ForwardIter>
__uninitialized_copy_n_aux(InputIter first, Size n, ForwardIter res, input_iterator_tag) {
    ForwardIter cur = res;
    for(;n > 0;--n, ++first, ++cur) {
        construct(&*cur, *first);
    }
    return pair<InputIter, ForwardIter>(first, cur);
}

template <class RandomAccessIter, class Size, class ForwardIter>
inline pair<RandomAccessIter, ForwardIter>
__uninitialized_copy_n_aux(RandomAccessIter first, Size n, ForwardIter res, random_access_iterator_tag) {
    RandomAccessIter last = first + n;
    return pair<RandomAccessIter, ForwardIter>(last, 
                uninitialized_copy(first, last, res));
}


// fill [first, last) with x
template <class ForwardIter, class T>
inline void uninitialized_fill(ForwardIter first, ForwardIter last, const T& x) {
    __uninitialized_fill_aux(first, last, x, is_POD_type<value_type<ForwardIter> >());
}

template <class ForwardIter, class T>
inline void __uninitialized_fill_aux(ForwardIter first, ForwardIter last, const T& x, false_type) {
    ForwardIter cur = first;
    for(;cur != last;++cur) {
        construct(&*cur, x);
    }
}

template <class ForwardIter, class T>
inline void __uninitialized_fill_aux(ForwardIter first, ForwardIter last, const T& x, true_type) {
    fill(first, last, x);
}

// fill [first, first + n) with x
template <class ForwardIter, class Size, class T>
inline ForwardIter uninitialized_fill_n(ForwardIter first, Size n, const T& x) {
    return __uninitialized_fill_n_aux(first, n, x, is_POD_type<value_type<ForwardIter> >());
}

template <class ForwardIter, class Size, class T>
inline ForwardIter __uninitialized_fill_n_aux(ForwardIter first, Size n, const T& x, false_type) {
    ForwardIter cur = first;
    for(;n > 0;--n, ++cur) {
        construct(&*cur, x);
    }
    return cur;
}

template <class ForwardIter, class Size, class T>
inline ForwardIter __uninitialized_fill_n_aux(ForwardIter first, Size n, const T& x, true_type) {
    fill_n(first, n, x);   
}




} // MiniSTL