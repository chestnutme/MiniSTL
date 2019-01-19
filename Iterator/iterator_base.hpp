#pragma once

#include <cstddef>

namespace MiniSTL {

// 5 kind of iterator, the class hierarchy
// the following five classes are the five kinds of return value 
// function iterator_category(), which will be used to argument
// deducation when overload resolution
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag : public input_iterator_tag {};
struct bidirectional_iterator_tag : public forward_iterator_tag {};
struct random_access_iterator_tag : public bidirectional_iterator_tag {};


template <class Iterator>
struct iterator_traits {
    // different kinds of iterator
    using iterator_category = typename Iterator::iterator_category;
    // the type of object pointed by iterator
    using value_type = typename Iterator::value_type;
    // the type of distance between 2 iterator
    using difference_type = typename Iterator::difference_type;
    // the address of object pointed by iterator
    using pointer = typename Iterator::pointer;
    // lvalue to mutate a iterator
    using reference = typename Iterator::reference;
};

// partial specialization for raw pointer of T
template <class T>
struct iterator_traits<T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using reference = T&;
};

// partial specializatoin for const raw pointer of T
template <class T>
struct iterator_traits<const T*> {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = const T*;
    using reference = const T&;
};

//alias template 
template <class Iterator>
using iterator_category_t = typename iterator_traits<Iterator>::iterator_category;

template <class Iterator>
using value_type_t = typename iterator_traits<Iterator>::value_type;

template <class Iterator>
using difference_type_t = typename iterator_traits<Iterator>::difference_type;

template <class Iterator>
using pointer_t = typename iterator_traits<Iterator>::pointer;

template <class Iterator>
using reference_t = typename iterator_traits<Iterator>::reference;


// distance func
template <class InputIterator>
inline difference_type_t<InputIterator> 
distance(InputIterator first, InputIterator last) {
    return __distance(first, last, iterator_category_t<InputIterator>());
}

template <class RandomAccessIterator>
inline difference_type_t<RandomAccessIterator>
__distance(RandomAccessIterator first, RandomAccessIterator last,
            random_access_iterator_tag) {
    return last - first;
}

template <class InputIterator>
inline difference_type_t<InputIterator>
__distance(InputIterator first, InputIterator last, 
            input_iterator_tag) {
    difference_type_t<InputIterator> n = 0;
    for(;first != last; ++n, ++first);
    return n;
}


// advance func
template <class InputIterator, class Distance>
inline void advance(InputIterator& i, Distance n) {
    __advance(i, n, iterator_category_t<InputIterator>());
}

template <class RandomAccessIterator, class Distance>
inline void __advance(RandomAccessIterator& i, Distance n,
                        random_access_iterator_tag) {
    i += n;
}

template <class BidirectionalIterator, class Distance>
inline void __advance(BidirectionalIterator& i, Distance n,
                        bidirectional_iterator_tag) {
    if (n >= 0) 
        while(n--) ++i;
    else 
        while(n++) --i;
}

template <class InputIterator, class Distance>
inline void __advance(InputIterator& i, Distance n,
                        input_iterator_tag) {
    while(n--) ++i;
}

} // MiniSTL