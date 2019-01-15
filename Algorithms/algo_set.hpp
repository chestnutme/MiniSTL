#pragma once

#include "../Iterator/iterator_base.hpp"
#include "../Function/function_base.hpp"
#include "../Util/pair.hpp"
#include "algobase.hpp"

namespace MiniSTL {

// set algorithms:
// precondition: input ranges needs to be sorted
// postcondition: output ranges will be sorted

// includes: return true if range [first2, last2) 
//           includes range [first1, last1)
template <class InputIt1, class InputIt2, 
            class Compare = less<value_type<InputIt1>> >
bool includes(InputIt1 first1, InputIt1 last1,
              InputIt2 first2, InputIt2 last2,
              Compare comp = Compare()) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first1))
            return false;
        else if(comp(*first1, first2)) 
            ++first1;
        else
            ++first1, ++first2;
    }
    return first2 == last2;
}


template <class InputIt1, class InputIt2, class OutputIt,
            class Compare = less<value_type<InputIt1>> >
OutputIt set_union(InputIt1 first1, InputIt1 last1,
                   InputIt2 first2, InputIt2 last2,
                   OutputIt result,
                   Compare comp = Compare()) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1,*first2)) {
            *result = *first1;
            ++first1;
        }
        else if (comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
        }
        else {
            *result = *first1;
            ++first1;
            ++first2;
        }
        ++result;
    }
    return copy(first2, last2, copy(first1, last1, result));
}


template <class InputIt1, class InputIt2, class OutputIt,
          class Compare = less<value_type<InputIt1>> >
OutputIt set_intersection(InputIt1 first1, InputIt1 last1,
                          InputIt2 first2, InputIt2 last2,
                          OutputIt result,
                          Compare comp = Compare()) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) 
            ++first1;
        else if (comp(*first2, *first1)) 
            ++first2;
        else {  
            *result = *first1;
            ++first1;
            ++first2;
            ++result;
        }
    }
    return result;
}

// set_difference: return [first1, last1) - [first2 - last2)
template <class InputIt1, class InputIt2, class OutputIt,
          class Compare = less<value_type<InputIt1>> >
OutputIt set_difference(InputIt1 first1, InputIt1 last1,
                        InputIt2 first2, InputIt2 last2,
                        OutputIt result,
                        Compare comp = Compare()) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (comp(*first2, *first1)
            ++first2;
        else {
            ++first1;
            ++first2;
        }
    }
    return copy(first1, last1, result);
}


template <class InputIt1, class InputIt2, class OutputIt,
          class Compare = less<value_type<InputIt1>> >
OutputIt 
set_symmetric_difference(InputIt1 first1, InputIt1 last1,
                         InputIt2 first2, InputIt2 last2,
                         OutputIt result,
                         Compare comp = Compare()) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
            ++result;
        }
        else if (comp(*first2, *first1)) {
            *result = *first2;
            ++first2;
            ++result;
        }
        else {
            ++first1;
            ++first2;
        }
    }
    return copy(first2, last2, copy(first1, last1, result));
}

template <class InputIt1, class InputIt2, class OutputIt, 
          class Equal = equal_to<value_type<InputIt1>>>
OutputIt
set_cartesian_product(InputIt1 first1, InputIt1 last1,
                      InputIt2 first2, InputIt2 last2,
                      OutputIt result,
                      Equal equal = Equal()) {
    while(first1 != last1) {
        while(first2 != last2) {
            *result = make_pair(*first1, first2);
            while(equal(*first2, *(++first2)) && first2 != last2);
            ++result;
        }
        while(equal(*first1, *(++first1)) && first1 != last1);
    }
    return result;
}


// bag algorithms:
// precondition: input ranges needs to be sorted
// postcondition: output ranges will be sorted

// includes: return true if range [first2, last2) 
//           includes range [first1, last1)
// for bag, includes means that bag A has the same element x n times,
// B has x m times, B includes A only if for each x, m >= n;
template <class InputIt1, class InputIt2, 
          class Compare = less<value_type<InputIt1>>,
          class Equal = equal_to<value_type<InputIt1>> >
bool bag_includes(InputIt1 first1, InputIt1 last1,
                  InputIt2 first2, InputIt2 last2,
                  Compare comp = Compare(),
                  Equal equal = Equal()) {
    while (first1 != last1 && first2 != last2) {
        if (comp(*first2, *first1))
            return false;
        else if(comp(*first1, first2)) 
            ++first1;
        else {
            int cnt1 = 0, cnt2 = 0;
            while(equal(*first2, *(++first2)) && first2 != last2)
                cnt2++;
            while(equal(*first1, *(++first1)) && first1 != last1) {
                cnt1++;
                if(cnt1 > cnt2)
                    return false;
            }
        }
    }
    return first2 == last2;
}

// for bag, union means that bag A has the same element x n times,
// B has x m times, result has x max(n, m) times
template <class InputIt1, class InputIt2, class OutputIt,
          class Compare = less<value_type<InputIt1>> >
using bag_union = set_union<InputIt1, InputIt2, OutputIt, Compare>;


// for bag, intersection means that bag A has the same element x n times,
// B has x m times, result has x min(n, m) times;
template <class InputIt1, class InputIt2, class OutputIt,
          class Compare = less<value_type<InputIt1>> >
using bag_intersection = set_intersection<InputIt1, InputIt2, 
                                          OutputIt, Compare>;

// bag_difference: return [first1, last1) - [first2 - last2)
// for bag, includes means that bag A has the same element x n times,
// B has x m times, result has x max(n - m , 0) times
template <class InputIt1, class InputIt2, class OutputIt,
          class Compare = less<value_type<InputIt1>> >
using bag_difference = set_difference<InputIt1, InputIt2,
                                      OutputIt, Compare>;


template <class InputIt1, class InputIt2, class OutputIt,
          class Compare = less<value_type<InputIt1>> >
using bag_symmetric_difference = 
            set_symmetric_difference<InputIt1, InputIt2,
                                     OutputIt, Compare>;

} // MiniSTL