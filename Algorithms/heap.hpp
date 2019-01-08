#pragma once

#include "../Iterator/iterator_base.hpp"

namespace MiniSTL {

template <class RandomIt>
void push_heap(RandomIt first, RandomIt last);

template <class RandomIt, class Compare>
void push_heap(RandomIt first, RandomIt last, Compare comp);

template< class RandomIt >
void pop_heap( RandomIt first, RandomIt last );

template< class RandomIt, class Compare >
void pop_heap( RandomIt first, RandomIt last, Compare comp );

template< class RandomIt >
void make_heap( RandomIt first, RandomIt last );

template< class RandomIt, class Compare >
void make_heap( RandomIt first, RandomIt last, Compare comp );

template <class RandomIt>
void sort_heap(RandomIt first, RandomIt last);

template <class RandomIt, class Compare>
void sort_heap(RandomIt first, RandomIt last, Compare comp);
        
template< class RandomIt >
bool is_heap( RandomIt first, RandomIt last );

template< class RandomIt, class Compare >
bool is_heap( RandomIt first, RandomIt last, Compare comp );

template< class RandomIt >
RandomIt is_heap_until( RandomIt first, RandomIt last );

template< class RandomIt, class Compare >
RandomIt is_heap_until( RandomIt first, RandomIt last, Compare comp );



} // MiniSTL

