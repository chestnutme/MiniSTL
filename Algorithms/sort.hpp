#pragma once 

#include "algo.hpp"
#include "algobase.hpp"
#include "heap.hpp"
#include "Util/tempbuf.hpp"
#include "Function/function_base.hpp"
#include "Traits/type_traits.hpp"

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
// using buf as temporary storage if len <= buf_size
// compared to inplace stable partition, recursion depth
// reduces massively
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

// using different stragegy when length less or greater
// then threshold
const int THRESHOLD = 16;

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

// if insert before first, using copy backward [first, last)
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
    if(last - first > THRESHOLD) {
        __insertion_sort(first, first + THRESHOLD, comp);
        __unguarded_insertion_sort(first + THRESHOLD, last, comp);
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
    while(last - first > THRESHOLD) {
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


// stable_sort()

// inplace sort:
// if lenth < 15 using insertion sort
// else sort[first, mid) and [mid, last)
// then merge the two
template <class RandomIt, 
        class Compare = less<value_type_t<RandomIt>>>
void __inplace_stable_sort(RandomIt first,
                           RandomIt last, Compare comp) {
    if(last - first < 15) {
        __insertion_sort(first, last, comp);
        return;
    }
    RandomIt mid = first + (last - first) / 2;
    __inplace_stable_sort(first, mid, comp);
    __inplace_stable_sort(mid, last, comp)
    __merge_without_buffer(first, mid, last,
                           mid - first, last - mid,
                           .comp);
}

template <class RandomIt1, class RandomIt2, class Distance, 
          class Compare = less<value_type_t<RandomIt1>>>
void __merge_sort_loop(RandomIt1 first, RandomIt1 last, 
                       RandomIt2 result, Distance step,
                       Compare comp = Compare()) {
    Distance two_step = 2 * step;

    while(last - first >= two_step) {
        result = merge(first, first + step,
                       first + step, first + two_step,
                       result, comp);
        first += two_step;
    }

    step = min(Distance(last - first), step);

    merge(first, first + step, first + step, last, result, comp);
}

// using as fragment size when insert sorting
const int CHUNK_SIZE = 7;

// chunk insertion sort: insert sort chunk-sized fragment
template <class RandomIt, class Distance, 
          class Compare = less<value_type_t<RandomIt>>>
void __chunk_insertion_sort(RandomIt first, RandomIt last,
                            Distance chunk, Compare comp)
{
    while(last - first >= chunk) {
        __insertion_sort(first, first + chunk, comp);
        first += chunk;
    }
    __insertion_sort(first, last, comp);
}

// first insert sort chunk-sized fragments
// then merge sort times of chunk-sized fragments using buf
template <class RandomIt, class Pointer,
          class Compare = less<value_type_t<RandomIt>>>
void __merge_sort_with_buffer(RandomIt first, RandomIt last, 
                              Pointer buf,
                              Compare comp = Compare()) {
    using Distance = difference_type_t<RandomIt>
    Distance len = last - first;
    Pointer buf_last = buf + len;

    Distance step = CHUNK_SIZE;
    __chunk_insertion_sort(first, last, step, comp);

    while(step < len) {
        __merge_sort_loop(first, last, buf, step, comp);
        step *= 2;
        __merge_sort_loop(buf, buf_last, first, step, comp);
        step *= 2;
    }
}

template <class InputIt1, class InputIt2, class OutputIt,
          class Compare = less<value_type_t<InputIt1>>>
OutputIt merge(InputIt1 first1, InputIt1 last1,
               InputIt2 first2, InputIt2 last2,
               OutputIt result, 
               Compare comp = Compare()) {
    while (first1 != last1 && first2 != last2) {
        if(comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
        } else {
            *result = *first1;
            ++first1;
        }
        ++result;
    }
    return copy(first2, last2, copy(first1, last1, result));
}

// inplace_merge

// merge by divided range into 2 fragments recursively
template <class BiIt, class Distance, class Compare>
void __merge_without_buffer(BiIt first, BiIt mid, BiIt last,
                            Distance len1, Distance len2,
                            Compare comp = Compare()) {
    if(len1 == 0 || len2 == 0)
        return;
    if(len1 + len2 == 2) {
        if(comp(*mid, *first))
            iter_swap(first, mid);
        return;
    }
    BiIt cut1 = first;
    BiIt cut2 = mid;
    Distance len11(0);
    Distance len22(0);
    if(len1 > len2) {
        len11 = len1 / 2;
        advance(cut1, len11);
        cut2 = lower_bound(mid, last, *cut1, comp);
        len22 = distance(mid, cut2)
    } else {
        len22 = len2 / 2;
        advance(cut2, len22);
        cut1 = upper_bound(first, mid, *cut2, comp);
        len11 = distance(first, cut1);
    }
    BiIt new_mid = rotate(cut1, mid, cut2);
    __merge_without_buffer(first, cut1, new_mid, len11, len22, comp);
    __merge_without_buffer(new_mid, cut2, last, len1 - len11,
                           len2 - len22, comp);
}

template <class BiIt1, class BiIt2, class BiIt3, 
          class Compare = less<value_type_t<BiIt1>>>
BiIt3 __merge_backward(BiIt1 first1, BiIt1 last1,
                       BiIt2 first2, BiIt2 last2,
                       BiIt3 result,
                       Compare comp = Compare()) {
    if(first1 == last1)
        return copy_backward(first2, last2, result);
    if(first2 == last2)
        return copy_backward(first1, last1, result);
    --last1;
    --last2;
    while (true) {
        if(comp(*last2, *last1)) {
            *--result = *last1;
            if(first1 == last1)
                return copy_backward(first2, ++last2, result);
            --last1;
        } else {
            *--result = *last2;
            if(first2 == last2)
                return copy_backward(first1, ++last1, result);
            --last2;
        }
    }
}

template <class BiIt, class Distance, class Pointer,
          class Compare  = less<value_type_t<BiIt>>>
void __merge_adaptive(BiIt first, BiIt mid, BiIt last,
                      Distance len1, Distance len2,
                      Pointer buf, Distance buf_size,
                      Compare comp) {
    if(len1 <= len2 && len1 <= buf_size) {
        // case1: front seg < buf, copy front into buf
        // merge buf and back seg into [first, last)
        Pointer buf_end = copy(first, mid, buf);
        merge(buf, buf_end, mid, last, first, comp);
    } else if(len2 <= buf_size) {
        // case2: back seg < buf, copy back into buf
        // merge backward front and buf into [first, last)
        Pointer buf_end = copy(mid, last, buf);
        __merge_backward(first, mid, buf, buf_end, last,
                        comp);
    } else {
        // case3: divided into 4 fragments and rotate 2nd and 3rd
        // then front half <= cut1, back half > cut1
        // recursively merge front and back with buf
        BiIt cut1 = first;
        BiIt cut2 = mid;
        Distance len11 = 0;
        Distance len22 = 0;
        if(len1 > len2) {
            len11 = len1 / 2;
            advance(cut1, len11);
            cut2 = lower_bound(mid, last, *cut1, comp);
            len22 = distance(mid, cut2);   
        } else {
            len22 = len2 / 2;
            advance(cut2, len22);
            cut1 = upper_bound(first, mid, *cut2, comp);
            len11 = distance(first, cut1);
        }
        BiIt new_mid =
            __rotate_adaptive(cut1, mid, cut2, len1 - len11,
                            len22, buf, buf_size);
        __merge_adaptive(first, cut1, new_mid, len11,
                         len22, buf, buf_size, comp);
        __merge_adaptive(new_mid, cut2, last, len1 - len11,
                         len2 - len22, buf, buf_size, comp);
    }
}

template <class BiIt, class Distance, 
          class Compare = less<value_type_t<BiIt>>>
inline void __inplace_merge_aux(BiIt first, BiIt mid, BiIt last,
                                Compare comp = Compare()) {
    using Distance = difference_type_t<BiIt>;                                    
    Distance len1 = distance(first, mid);
    Distance len2 = distance(mid, last);

    Temporary_Buffer<BiIt, value_type_t<BiIt>> buf(first, last);
    if(buf.begin() == 0)
        __merge_without_buffer(first, mid, last, len1, len2, comp);
    else
        __merge_adaptive(first, mid, last, len1, len2,
                         buf.begin(), Distance(buf.size()), comp);
}

template <class BiIt, 
          class Compare = less<value_type_t<BiIt>>>
inline void inplace_merge(BiIt first, BiIt mid, BiIt last, 
                          Compare comp = Compare()) {

    if(first == mid || mid == last)
        return;
    __inplace_merge_aux(first, mid, last, comp);
}

// if half > buf_size, recursively stable sort
// else merge sort with buf finally merge front, back half
template <class RandomIt, class Pointer, class Distance, 
          class Compare = less<value_type_t<RandomIt>>>
void __stable_sort_adaptive(RandomIt first, RandomIt last, 
                            Pointer buf, Distance buf_size, 
                            Compare comp = Compare()) {
    Distance len = (last - first + 1) / 2;
    RandomIt mid = first + len;
    if(len > buf_size) {
        __stable_sort_adaptive(first, mid, buf, buf_size, comp);
        __stable_sort_adaptive(mid, last, buf, buf_size, comp);
    } else {
        __merge_sort_with_buffer(first, mid, buf, comp);
        __merge_sort_with_buffer(mid, last, buf, comp);
    }
    __merge_adaptive(first, mid, last, Distance(mid - first), 
                     Distance(last - mid), buf, buf_size,
                     comp);
}

// if has buf, using inplace insertion sort
// otherwise using stable sort with buf
template <class RandomIt,
          class Compare = less<value_type_t<RandomIt>>>
inline void stable_sort(RandomIt first, RandomIt last,
                              Compare comp = Compare()) {
    Temporary_Buffer<RandomIt, T> buf(first, last);
    if(buf.begin() == 0)
        __inplace_stable_sort(first, last, comp);
    else 
        __stable_sort_adaptive(first, last, buf.begin(),
                               buf.size(), comp);
}


// partial_sort, partial_sort_copy
template <class RandomIt, 
          class Compare = less<value_type_t<RandomIt>>>
void partial_sort(RandomIt first, RandomIt mid,
                    RandomIt last, Compare comp) {
    make_heap(first, mid, comp);
    for(RandomIt i = mid; i < last; ++i)
        if(comp(*i, *first))
            pop_heap(first, mid, i, *i, comp);
    sort_heap(first, mid, comp);
}

template <class InputIt, class RandomIt, 
          class Compare = less<value_type_t<InputIt>>>
RandomIt partial_sort_copy(InputIt first, InputIt last,
                             RandomIt result_first,
                             RandomIt result_last,
                             Compare comp = Compare()) {
    if(result_first == result_last)
        return result_last;
    RandomIt result_real_last = result_first;
    while(first != last && result_real_last != result_last) {
        *result_real_last = *first;
        ++result_real_last;
        ++first;
    }
    make_heap(result_first, result_real_last, comp);
    while(first != last) {
        if(comp(*first, *result_first))
            adjust_heap(result_first, Distance(0),
                        Distance(result_real_last - result_first),
                        *first, comp);
        ++first;
    }
    sort_heap(result_first, result_real_last, comp);
    return result_real_last;
}

} // MiniSTL
