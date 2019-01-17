#pragma once

#include "Iterator/iterator_base.hpp"
#include "Function/function_base.hpp"

namespace MiniSTL {

// push a element in heap at [first, last - 1)
// new element has been already positioned at last - 1
template <class RandomIt, class Compare = less<value_type<RandomIt>> >
void push_heap(RandomIt first, RandomIt last, 
               Compare comp = Compare()) {
    using Distance = difference_type<RandomIt>;
    using T = value_type<RandomIt>;
    __push_heap(first, Distance(last - first - 1), Distance(0), T(*(last - 1), comp);
}

// push a element into heap 
// [first + topIdx, first + holeIdx - 1)
template <class RandomIt, class Distance, class T, class Compare>
void __push_heap(RandomIt first, Distance holeIdx, Distance topIdx, 
                 T v, Compare comp = Compare()) {
    Distance parent = (holeIdx - 1) / 2;
    while(holeIdx > topIdx && comp(*(first + parent), v)) {
        *(first + holeIdx) = *(first + parent);
        holeIdx = parent;
        parent = (holeIdx - 1) / 2;
    }
    *(first + holeIdx) = v;
}

// pop the first element(max) from heap [first, last)
// after return, max will be positioned at last - 1;
template<class RandomIt, class Compare = less<value_type<RandomIt>> >
void pop_heap(RandomIt first, RandomIt last, 
              Compare comp = Compare()) {
    using T = value_type<RandomIt>;
    __pop_heap(first, last - 1, last - 1, T(*(last - 1)), comp)
}

template <class RandomIt, class T, class Compare>
void __pop_heap(RandomIt first, RandomIt last, RandomIt result,
                T v, Compare comp = Compare()) {
    using Distance = difference_type<RandomIt>;
    // max is positioned at last - 1
    // previous *(last - 1) is v;
    *result = *first; 
    adjust(first, Distance(0), Distance(last - first), v, comp)
}

// fill the holeIdx with max(left, right) until len - 1
template <class RandomIt, class Distance, class T, class Compare>
void adjust_heap(RandomIt first, Distance holeIdx, Distance len, 
                 T v, Compare comp = Compare()) {
    Distance topIdx = holeIdx;
    Distance child = 2 * holeIdx + 2;
    while(child < len) {
        if(comp(*(first + child), *(first, child - 1))) 
            child--;
        *(first + holeIdx) = *(first + child);
        holeIdx = child;
        child = 2 * holeIdx + 2;
    }
    // no right child
    if(child == len) {
        *(first + holeIdx) = *(first + child - 1);
        holeIdx = child - 1;
    }
    *(first + holeIdx) = v;
}

// turn [first, last) to a heap;
template< class RandomIt, class Compare = less<value_type<RandomIt>> >
void make_heap(RandomIt first, RandomIt last,
               Compare comp = Compare()) {
    using Distance = difference_type<RandomIt>;
    using T = value_type<RamdomIt>;
    if(last - first < 2) return;
    Distance len = last - first;
    // last node which has child
    Distance parent = (len - 2) / 2;
    while(parent > 0) {
        // adjust the subtree with root parent
        adjust_heap(first, parent, len, 
                    T(*(first + parent)), comp);
        parent--;
    }
}

// sort with inc order
// require [first, last) already a heap
template <class RandomIt, class Compare = less<value_type<RandomIt>> >
void sort_heap(RandomIt first, RandomIt last, 
             Compare comp = Compare()) {
    while(last - first > 1)
        pop_heap(first, last--, comp);
}

// check range [first, last - 1) is heap or not
template< class RandomIt, class Compare = less<value_type<RandomIt>>>
bool is_heap(RandomIt first, RandomIt last, 
             Compare comp = Compare()) {
    if(last - first < 2) 
        return true;
    RandomIt until = is_heap_until(first, last, comp);
    if(until == last) 
        return true;
    return false;
}

// check range [first, last - 1) is heap or not
template< class RandomIt, class Compare = less<value_type<RandomIt>>>
RandomIt is_heap_until(RandomIt first, RandomIt last, 
                       Compare comp = Compare()) {
    using Distance = difference_type<RandomIt>;
    if(last - first < 2) return first;
    Distance parent = Distance(0);
    Distance len = Distance(last - first);
    // // check the subtree with left and right
    // while(parent < (len - 2) / 2) {
    //     Distance left = parent * 2 + 1;
    //     if(comp(*(first + parent), *(first + child))) 
    //         return first + child - 1;
    //     child++;
    //     if(comp(*(first + parent), *(first + child)))
    //         return first + child - 1;
    // }
    // // check the last left
    // Distance child = parent * 2 + 1;
    // if(comp(*(first + parent), *(first + child))) 
    //         return first + child - 1;
    // // if has the last right, check
    // if(parent * 2 + 2 < len) {
    //     child++;
    //     if(comp(*(first + parent), *(first + child))) 
    //         return first + child - 1;
    // }
    // return last;
    for(Distance child = 1;child < len;child++) {
        if(comp(*(first + parent), *(first + child)))
            return first + child - 1;
        if((child & 1) == 0))
            ++parent;
    }
    return last;
}


} // MiniSTL

