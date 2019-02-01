#pragma once

#include "Function/function_base.hpp"
#include "Iterator/iterator.hpp"
#include "algobase.hpp"


namespace MiniSTL {


// ---------------------------
// search algos

// find and find_if: return the first result
template <class InputIt, class T>
inline InputIt __find(InputIt first, InputIt last,
                      const T& val, input_iterator_tag) {
    while(first != last && !(*first == val))
        ++first;
    return first;
}

template <class InputIt, class Predicate>
inline InputIt __find_if(InputIt first, InputIt last,
                         Predicate pred, input_iterator_tag) {
    while(first != last && !pred(*first))
        ++first;
    return first;
}

template <class RandomIt, class T>
RandomIt __find(RandomIt first, RandomIt last,
              const T& val, random_access_iterator_tag) {
    difference_type_t<RandomIt> trip_cnt = (last - first) >> 2;

    for(;trip_cnt > 0;--trip_cnt) {
        if(*first == val) return first;
        ++first;

        if(*first == val) return first;
        ++first;

        if(*first == val) return first;
        ++first;

        if(*first == val) return first;
        ++first;
    }

    switch(last - first) {
    case 3:
        if(*first == val) return first;
        ++first;
    case 2:
        if(*first == val) return first;
        ++first;
    case 1:
        if(*first == val) return first;
        ++first;
    case 0:
    default:
        return last;
    }
}

template <class RandomIt, class Predicate>
RandomIt __find_if(RandomIt first, RandomIt last,
                   Predicate pred, random_access_iterator_tag) {
    difference_type_t<RandomIt> trip_cnt = (last - first) >> 2;

    for(;trip_cnt > 0 ; --trip_cnt) {
        if(pred(*first)) return first;
        ++first;

        if(pred(*first)) return first;
        ++first;

        if(pred(*first)) return first;
        ++first;

        if(pred(*first)) return first;
        ++first;
    }

    switch(last - first) {
    case 3:
        if(pred(*first)) return first;
        ++first;
    case 2:
        if(pred(*first)) return first;
        ++first;
    case 1:
        if(pred(*first)) return first;
        ++first;
    case 0:
    default:
        return last;
    }
}

template <class InputIt, class T>
inline InputIt find(InputIt first, InputIt last, const T& val) {
  return __find(first, last, val, iterator_category_t<InputIt>());
}

template <class InputIt, class Predicate>
inline InputIt find_if(InputIt first, InputIt last, Predicate pred) {
  return __find_if(first, last, pred, iterator_category_t<InputIt>());
}

// find_first_of
// Search [first2, last2) as a subsequence in [first1, last1), 
// and return the *first* possible match or last1 ifnot found
// optimize : use KMP or BM
template <class InputIt, class ForwardIt, 
          class BiPredicate = equal_to<value_type_t<InputIt>> >
InputIt find_first_of(InputIt first1, InputIt last1,
                      ForwardIt first2, ForwardIt last2,
                      BiPredicate pred = BiPredicate()) {
    for(;first1 != last1; ++first1) {
        for(ForwardIt iter = first2; iter != last2; ++iter) {
            if(pred(*first1, *iter))
                return first1;
        }
    }
    return last1;
}


// find_end:
// Search [first2, last2) as a subsequence in [first1, last1), and return
// the *last* possible match.  Note that find_end for bidirectional iterators
// is much faster than for forward iterators.

// find_end for forward iterators. 
template <class ForwardIt1, class ForwardIt2, class BiPredicate>
ForwardIt1 __find_end(ForwardIt1 first1, ForwardIt1 last1,
                      ForwardIt2 first2, ForwardIt2 last2,
                      forward_iterator_tag, forward_iterator_tag,
                      BiPredicate comp) {
    if(first2 == last2)
        return last1;
    else {
        ForwardIt1 result = last1;
        while(1) {
            ForwardIt1 new_result = search(first1, last1, 
                                            first2, last2, comp);
            if(new_result == last1)
                return result;
            else {
                result = new_result;
                first1 = new_result;
                ++first1;
            }
        }
    }
}

// find_end for bidirectional iterators: 
template <class BiIt1, class BiIt2, class BiPredicate>
BiIt1 __find_end(BiIt1 first1, BiIt1 last1,
                 BiIt2 first2, BiIt2 last2,
                 bidirectional_iterator_tag,
                 bidirectional_iterator_tag, 
                 BiPredicate comp = BiPrecidate()) {
    using ReIt1 = __reverse_iterator<BiIt1>;
    using ReIt2 = __reverse_iterator<BiIt2>;
    ReIt1 rlast1(first1);
    ReIt2 rlast2(first2);
    ReIt1 rresult = search(ReIt1(last1), rlast1,
                           ReIt2(last2), rlast2, comp);

    if(rresult == rlast1)
        return last1;
    else {
        BiIt1 result = rresult.base();
        advance(result, -distance(first2, last2));
        return result;
    }
}


// Dispatching functions for find_end.
template <class ForwardIt1, class ForwardIt2, 
          class BiPredicate = equal_to<value_type_t<ForwardIt1>> >
inline ForwardIt1 
find_end(ForwardIt1 first1, ForwardIt1 last1, 
         ForwardIt2 first2, ForwardIt2 last2,
         BiPredicate comp) {
  return find_end(first1, last1, first2, last2,
                  iterator_category<ForwardIt1>(),
                  iterator_category<ForwardIt2>(),
                  comp);
}

// adjacent_find : return the first 2 adjacent element meet pred
template <class ForwardIt, 
          class BiPredicate = equal_to<value_type_t<ForwardIt>> >
ForwardIt adjacent_find(ForwardIt first, ForwardIt last,
                        BiPredicate pred = BiPredicate()) {
    if(first == last)
        return last;
    ForwardIt next = first;
    while(++next != last) {
        if(pred(*first, *next))
            return first;
        first = next;
    }
    return last;
}

// count and count_if
template <class InputIt, class T>
difference_type_t<InputIt>
cnt(InputIt first, InputIt last, const T& val) {
    difference_type_t<InputIt> n = 0;
    for(;first != last; ++first) {
        if(*first == val)
            ++n;
    }
    return n;
}

template <class InputIt, class Predicate>
difference_type_t<InputIt>
count_if(InputIt first, InputIt last, Predicate pred) {
    difference_type_t<InputIt> n = 0;
    for(;first != last; ++first) {
        if(pred(*first))
            ++n;
    }
    return n;
}

// search
template <class ForwardIt1, class ForwardIt2, 
          class BiPredicate = equal_to<value_type_t<ForwardIt1>> >
ForwardIt1 search(ForwardIt1 first1, ForwardIt1 last1,
                  ForwardIt2 first2, ForwardIt2 last2,
                  BiPredicate pred = BiPredicate())  {
    difference_type_t<ForwardIt1> d1 = distance(first1, last1);
    difference_type_t<ForwardIt2> d2 = distance(first2, last2);

    if(d1 < d2 || d1 == difference_type_t<ForwardIt1>(0))
        return last1;
    
    ForwardIt1 cur1 = first1;
    ForwardIt2 cur2 = first2;

    while(cur2 != last2) {
        if(pred(*cur1, *cur2)) {
            ++cur1;
            ++cur2;
        } else {
            if(d1 == d2)
                return last1;
            else {
                cur1 = ++first1;
                cur2 = first2;
                --d1;
            }
        }
    }
    return first1;
}

// search_n.  Search for cnt consecutive copies of val.
template <class ForwardIt, class Integer, class T, 
          class BiPredicate = equal_to<value_type_t<ForwardIt>> >
ForwardIt search_n(ForwardIt first, ForwardIt last,
                   Integer cnt, const T& val,
                   BiPredicate pred = BiPredicate()) {
    if(cnt <= 0)
        return first;

    while(first != last) {
        if(pred(*first, val))
            break;
        ++first;
    }
    while(first != last) {
        Integer n = cnt - 1;
        ForwardIt i = first;
        ++i;
        while(i != last && n != 0 && pred(*i, val)) {
            ++i;
            --n;
        }
        if(n == 0)
            return first;
        else {
            while(i != last) {
                if(pred(*i, val))
                    break;
                ++i;
            }
            first = i;
        }
    }
    return last;
} 

// Binary search (lower_bound, upper_bound, equal_range, binary_search).

template <class ForwardIt, class T, 
          class Compare = less<T> >
inline ForwardIt lower_bound(ForwardIt first, ForwardIt last,
                             const T& val,
                             Compare comp = Compare()) {
    difference_type_t<ForwardIt> len = distance(first, last);
    difference_type_t<ForwardIt> half;
    ForwardIt mid;

    while(len > 0) {
        half = len >> 1;
        mid = first;
        advance(mid, half);
        if(comp(*mid, val)) {
            first = mid;
            ++first;
            len = len - half - 1;
        }
        else
            len = half;
    }
    return first;
}

template <class ForwardIt, class T,
          class Compare = less<T> >
inline ForwardIt upper_bound(ForwardIt first, ForwardIt last,
                             const T& val,
                             Compare comp = Compare()) {
    difference_type_t<ForwardIt> len = distance(first, last);
    difference_type_t<ForwardIt> half;
    ForwardIt mid;

    while (len > 0) {
        half = len >> 1;
        mid = first;
        advance(mid, half);
        if (comp(val, *mid))
            len = half;
        else {
            first = mid;
            ++first;
            len = len - half - 1;
        }
    }
    return first;
}


template <class ForwardIt, class T, 
          class Compare = less<T> >
pair<ForwardIt, ForwardIt>
equal_range(ForwardIt first, ForwardIt last, const T& val,
            Compare comp = Compare()) {
    difference_type_t<ForwardIt> len = distance(first, last);
    difference_type_t<ForwardIt> half;
    ForwardIt mid, left, right;

    while (len > 0) {
        half = len >> 1;
        mid = first;
        advance(mid, half);
        if (comp(*mid, val)) {
            first = mid;
            ++first;
            len = len - half - 1;
        }
        else if (comp(val, *mid))
            len = half;
        else {
            left = lower_bound(first, mid, val, comp);
            advance(first, len);
            right = upper_bound(++mid, first, val, comp);
            return pair<ForwardIt, ForwardIt>(left, right);
        }
    }
    return pair<ForwardIt, ForwardIt>(first, first);
}           

template <class ForwardIt, class T, 
          class Compare = less<T> >
bool binary_search(ForwardIt first, ForwardIt last,
                   const T& val,
                   Compare comp = Compare()) {
    ForwardIt i = lower_bound(first, last, val, comp);
    return i != last && !comp(val, *i);
}


//------------------------
// other misc

// median : median of three number
template <class T, class Compare = less<T> >
inline const T&
median(const T& a, const T& b, const T& c, Compare comp = Compare()) {
    if(comp(a, b)) {
        if(comp(b, c))
            return b;
        else if(comp(a, c))
            return c;
        else
            return a;
    } else if(comp(a, c))
        return a;
    else if(comp(b, c))
        return c;
    else
        return b;
}

// max_element
template <class ForwardIt,
          class Compare = less<value_type_t<ForwardIt>> >
ForwardIt max_element(ForwardIt first, ForwardIt last,
			          Compare comp = Compare()) {
    if(first == last)
        return first;
    ForwardIt result = first;
    while(++first != last) {
        if(comp(*result, *first))
            result = first;
    }
    return result;
}

// min_element
template <class ForwardIt,
          class Compare = less<value_type_t<ForwardIt>> >
ForwardIt min_element(ForwardIt first, ForwardIt last,
			          Compare comp = Compare()) {
    if(first == last)
        return first;
    ForwardIt result = first;
    while(++first != last) {
        if(comp(*first, *result))
            result = first;
    }
    return result;
}

// for_each : Apply a function to every element of a range.
template <class InputIt, class Function>
Function for_each(InputIt first, InputIt last, Function f) {
    for(;first != last; ++first)
        f(*first);
    return f;
}

// swap_ranges
template <class ForwardIt1, class ForwardIt2>
ForwardIt2 swap_ranges(ForwardIt1 first1, ForwardIt1 last1,
                          ForwardIt2 first2) {
    for(;first1 != last1;++first1, ++first2)
        iter_swap(first1, first2);
    return first2;
}

// transform
template <class InputIt, class OutputIt, class UnaryOp>
OutputIt transform(InputIt first, InputIt last,
                   OutputIt result, UnaryOp opr) {
    for(;first != last; ++first, ++result)
        *result = opr(*first);
    return result;
}

template <class InputIt1, class InputIt2, class OutputIt,
          class BinaryOp>
OutputIt transform(InputIt1 first1, InputIt1 last1,
                   InputIt2 first2, OutputIt result,
                   BinaryOp binary_op) {
    for(;first1 != last1; ++first1, ++first2, ++result)
        *result = binary_op(*first1, *first2);
    return result;
}

} // MiniSTL