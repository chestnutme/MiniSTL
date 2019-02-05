#pragma once 

#include "algo.hpp"
#include "algobase.hpp"
#include "Util/tempbuf.hpp"
#include "Function/function_base.hpp"


namespace MiniSTL {

// partition, stable_partition, and their auxiliary functions
template <class ForwardIt, class Predicate>
ForwardIt __partition(ForwardIt first,  ForwardIt last,
			          Predicate pred, forward_iterator_tag) {
    if(first == last)
        return first;
    while(pred(*first))
        if(++first == last) return first;

    ForwardIt next = first;
    while(++next != last) {
        if(pred(*next)) {
            swap(*first, *next);
            ++first;
        }
    }

    return first;
}

template <class BiIt, class Predicate>
BiIt __partition(BiIt first, BiIt last,
			     Predicate pred, bidirectional_iterator_tag) {
    while(true) {
        while(true) {
            if(first == last)
                return first;
            else if(pred(*first))
                ++first;
            else
                break;
        }
        --last;
        while(true) {
            if(first == last)
                return first;
            else if(!pred(*last))
                --last;
            else
                break;
        }
        iter_swap(first, last);
        ++first;
    }
}

template <class ForwardIt, class Predicate>
inline ForwardIt partition(ForwardIt first, ForwardIt last,
			               Predicate pred) {
    return __partition(first, last, pred, 
                       iterator_category_t<ForwardIt>());
}


template <class ForwardIt, class Predicate, class Distance>
ForwardIt __inplace_stable_partition(ForwardIt first, ForwardIt last,
                                     Predicate pred, Distance len) {
    if(len == 1)
        return pred(*first) ? last : first;
    ForwardIt mid = first;
    advance(mid, len / 2);
    return rotate(__inplace_stable_partition(first, mid, pred, 
                                             len / 2),
                  mid,
                  __inplace_stable_partition(mid, last, pred,
                                             len - len / 2));
}

// partition with stable order
// using buf as temporary storage iflen <= buf_size
template <class ForwardIt, class Pointer, class Predicate, 
          class Distance>
ForwardIt
__stable_partition_adaptive(ForwardIt first, ForwardIt last,
                            Predicate pred, Distance len,
                            Pointer buf, Distance buf_size) {
    if(len <= buf_size) {
        ForwardIt result1 = first;
        Pointer result2 = buf;
        for(;first != last;++first) {
            if(pred(*first)) {
                *result1 = *first;
                ++result1;
            } else {
                *result2 = *first;
                ++result2;
            }
        }
        copy(buf, result2, result1);
        return result1;
    } else {
        ForwardIt mid = first;
        advance(mid, len / 2);
        return rotate(__stable_partition_adaptive(
                            first, mid, pred,
                            len / 2, buf, buf_size),
                      mid,
                      __stable_partition_adaptive(
                            mid, last, pred,
                            len - len / 2, buf, buf_size));
    }
}

template <class ForwardIt, class Predicate>
inline ForwardIt
__stable_partition_aux(ForwardIt first, ForwardIt last, 
                       Predicate pred) {
    using Distance = difference_type_t<ForwardIt>;
    Temporary_Buffer<ForwardIt, value_type_t<ForwardIt>> buf(first, last);
    if(buf.size() > 0)
        return __stable_partition_adaptive(
                    first, last, pred,
                    Distance(buf.requested_size()),
                    buf.begin(), buf.size());
    else
        return __inplace_stable_partition(
                    first, last, pred, 
                    Distance(buf.requested_size()));
}

template <class ForwardIt, class Predicate>
inline ForwardIt 
stable_partition(ForwardIt first, ForwardIt last, 
                 Predicate pred) {
    if(first == last)
        return first;
    else
        return __stable_partition_aux(first, last, pred);
}

template <class RandomIt, class T, class Compare = less<T>>
RandomIt __unguarded_partition(RandomIt first, RandomIt last, 
                               T pivot, Compare comp = Comapre()) {
    while(true) {
        while(comp(*first, pivot))
            ++first;
        --last;
        while(comp(pivot, *last))
            --last;
        if(!(first < last))
            return first;
        iter_swap(first, last);
        ++first;
    }
}

const int threshold = 16;

// sort()

// insert by move element after pos one by one in [.., last)
template <class RandomIt, class T, class Compare = less<T>>
void __unguarded_linear_insert(RandomIt last, T val, 
                               Compare comp = Comapre()) {
    RandomIt next = last;
    --next;  
    while(comp(val, *next)) {
        *last = *next;
        last = next;
        --next;
    }
    *last = val;
}

// ifinsert before first, using copy backward [first, last)
template <class RandomIt, 
          class Compare = less<value_type_t<RandomIt>>>
inline void __linear_insert(RandomIt first, RandomIt last, 
                            Compare comp = Compare()) {
    value_type_t<ForwardIt> val = *last;
    if(comp(val, *first)) {
        copy_backward(first, last, last + 1);
        *first = val;
    } else
        __unguarded_linear_insert(last, val, comp);
}

template <class RandomIt, 
          class Compare = less<value_type_t<RandomIt>>>
void __insertion_sort(RandomIt first, RandomIt last, 
                      Compare comp = Compare()) {
    if(first == last)
        return;
    for(RandomIt i = first + 1; i != last; ++i)
        __linear_insert(first, i, comp);
}

template <class RandomIt,
          class Compare = less<value_type_t<RandomIt>>>
void __unguarded_insertion_sort(RandomIt first, RandomIt last,
                                Compare comp = Compare()) {
    for(RandomIt i = first; i != last; ++i)
        __unguarded_linear_insert(i, *i, comp);
}

template <class RandomIt, 
          class Compare = less<value_type_t<RandomIt>>>
void __final_insertion_sort(RandomIt first, RandomIt last, 
                            Compare comp = Compare()) {
    if(last - first > threshold) {
        __insertion_sort(first, first + threshold, comp);
        __unguarded_insertion_sort(first + threshold, last, comp);
    } else
        __insertion_sort(first, last, comp);
}

template <class Size>
inline Size __lg(Size n) {
    Size k;
    for(k = Size(0); n != 1; n >>= 1)
        ++k;
    return k;
}
 
// depth_limit means recursion stack depth
// ifstack >= limit, using partial_sort(heap sort)
// else partition to [first, cut), [cut, last)
// then introsort[cut, last), introsort[first, cut)
template <class RandomIt, class T, class Size, 
          class Compare = less<value_type_t<RandomIt>>>
void __introsort_loop(RandomIt first, RandomIt last,
                      Size depth_limit, Compare comp = Compare()) {
    while(last - first > threshold) {
        if(depth_limit == 0) {
            partial_sort(first, last, last, comp);
            return;
        }
        --depth_limit;
        RandomIt cut =
        __unguarded_partition(
            first, last,
             __median(*first,
                      *(first + (last - first)/2),
                      *(last - 1), comp)),
            comp);
        __introsort_loop(cut, last, depth_limit, comp);
        __introsort_loop(first, last, depth_limit, comp);
    }
}

// first : introsort_loop partitions range as size-16 sorted fragments
// second : insert sort these fragments
// note: insert sort has beneficits when range is "short" or "almost sorted"
template <class RandomIt,
          class Compare = less<value_type_t<RandomIt>>>
inline void sort(RandomIt first, RandomIt last,
                 Compare comp = Compare()) {
    if(first != last) {
        __introsort_loop(first, last,
                         __lg(last - first) * 2,
                         comp);
        __final_insertion_sort(first, last, comp);
    }
}

} // MiniSTL
