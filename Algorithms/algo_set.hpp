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
        else if (*first2 < *first1)
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
          class Equal = equal_to<value_type<InputIt1>> >
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


} // MiniSTL