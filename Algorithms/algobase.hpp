#pragma once

#include <cstddef> // ptrdiff_t
#include <cstring> // memmove, memcmp
#include "Iterator/iterator_base.hpp"
#include "Traits/type_traits.hpp"
#include "Util/pair.hpp"

#include <iostream>


namespace MiniSTL {

//-------------------------------------------------
// swap and iter_swap

template <class ForwardIt1, class ForwardIt2>
inline void iter_swap(ForwardIt1 x, ForwardIt2 y) {
    value_type_t<ForwardIt1> tmp = *x;
    *x = *y;
    *y = tmp;
}

template <class T>
inline void swap(T& x, T& y) {
    T tmp = x;
    x = y;
    y = tmp;
}

//--------------------------------------------------
// min and max


template <class T>
inline const T& min(const T& x, const T& y) {
    return y < x ? y : x;
}

template <class T>
inline const T& max(const T& x, const T& y) {
    return  x < y ? y : x;
}

template <class T, class Compare>
inline const T& min(const T& x, const T& y, Compare comp) {
    return comp(y, x) ? y : x;
}

template <class T, class Compare>
inline const T& max(const T& x, const T& y, Compare comp) {
    return comp(x, y) ? y : x;
}

//--------------------------------------------------
// copy

// All of these auxiliary functions serve two purposes.  
//   1. Replace calls to copy with memmove whenever possible.  (Memmove, not memcpy, 
//      because the input and output ranges are permitted to overlap.)
//   2. If using random access iterators, then write the loop as
//      a for loop with an explicit count n

// for input iter, assign one by one, use first == last as end condition
template <class InputIt, class OutputIt, class Distance>
inline OutputIt __copy(InputIt first, InputIt last,
                        OutputIt result,
                        input_iterator_tag) {
    std::cout << "copy input iter" << std::endl;
    for ( ; first != last; ++result, ++first)
        *result = *first;
    return result;
}

// for random iter, assign one by one, use n == 0 as end condition
template <class RandomIt, class OutputIt, class Distance>
inline OutputIt
__copy(RandomIt first, RandomIt last, OutputIt result, 
        random_access_iterator_tag) {
    std::cout << "copy random iter" << std::endl;
    for (Distance n = last - first; n > 0; --n) {
        *result = *first;
        ++first;
        ++result;
    }
    return result;
}

// for raw pointer, use memmove
template <class T>
inline T*
__copy_trivial(const T* first, const T* last, T* result) {
    std::cout << "copy trivial" << std::endl;
    memmove(result, first, sizeof(T) * (last - first));
    return result + (last - first);
}

template <class InputIt, class OutputIt>
inline OutputIt copy(InputIt first, InputIt last,
                        OutputIt result) {
    using trivial = has_trivial_assignment_operator_t<value_type_t<InputIt> >;
    return __copy_aux(first, last, result, trivial());
}

template <class InputIt, class OutputIt>
inline OutputIt __copy_aux(InputIt first, InputIt last,
                               OutputIt result, false_type) {
    return __copy(first, last, result,
                  iterator_category_t<InputIt>());
}

template <class InputIt, class OutputIt>
inline OutputIt __copy_aux(InputIt first, InputIt last,
                               OutputIt result, true_type) {
    return __copy(first, last, result,
                  iterator_category_t<InputIt>());
}

// for raw pointer
template <class T>
inline T* __copy_aux(const T* first, const T* last, T* result,
                        true_type) {
    return __copy_trivial(first, last, result);
}


//--------------------------------------------------
// copy_backward

template <class T> 
inline T* __copy_backward_trivial(const T* first, const T* last, 
                                T* result){
    const ptrdiff_t length = last - first;
    memmove(result - length, first, sizeof(T) * length);
    return result - length;
}

template <class BiIt1, class BiIt2, 
          class Distance>
inline BiIt2 __copy_backward(BiIt1 first, BiIt1 last, 
                              BiIt2 result,
                              bidirectional_iterator_tag) {
    while (first != last)
        *--result = *--last;
    return result;
}

template <class RandomIt, class BiIt, class Distance>
inline BiIt __copy_backward(RandomIt first, RandomIt last, 
                             BiIt result,
                             random_access_iterator_tag) {
    for (Distance n = last - first; n > 0; --n)
        *--result = *--last;
    return result;
}

template <class BiIt1, class BiIt2>
inline BiIt2 copy_backward(BiIt1 first, BiIt1 last, BiIt2 result) {
    using trivial = has_trivial_assignment_operator_t<value_type_t<BiIt1> >;
    return __copy_backward_aux(first, last, result, trivial());
}

template <class BiIt1, class BiIt2>
inline BiIt2 __copy_backward_aux(BiIt1 first, BiIt1 last, 
                                  BiIt2 result, true_type) {
    return __copy_backward(first, last, result, 
                            iterator_category_t<BiIt1>());
}

template <class BiIt1, class BiIt2>
inline BiIt2 __copy_backward_aux(BiIt1 first, BiIt1 last, 
                                  BiIt2 result, false_type) {
    return __copy_backward(first, last, result,
                            iterator_category_t<BiIt1>());
}

template <class T>
inline T* __copy_backward_aux(const T* first, const T* last, 
                                T* result, true_type) {
    return __copy_backward_trivial(first, last, result);
}


//--------------------------------------------------
// copy_n 

template <class InputIt, class Size, class OutputIt>
inline pair<InputIt, OutputIt>
copy_n(InputIt first, Size count, OutputIt result) {
    return __copy_n(first, count, result,
                        iterator_category_t<InputIt>());
}

template <class InputIt, class Size, class OutputIt>
pair<InputIt, OutputIt> __copy_n(InputIt first, Size count,
                                    OutputIt result,
                                    input_iterator_tag) {
    for (;count > 0;--count) {
        *result = *first;
        ++first;
        ++result;
    }
    return pair<InputIt, OutputIt>(first, result);
}

template <class RandomIt, class Size, class OutputIt>
inline pair<RandomIt, OutputIt>
__copy_n(RandomIt first, Size count, OutputIt result,
            random_access_iterator_tag) {
    RandomIt last = first + count;
    return pair<RandomIt, OutputIt>(last, copy(first, last, result));
}

//--------------------------------------------------
// fill and fill_n

template <class ForwardIt, class T>
void fill(ForwardIt first, ForwardIt last, const T& __value) {
    for (;first != last;++first)
        *first = __value;
}

template <class OutputIt, class Size, class T>
OutputIt fill_n(OutputIt first, Size n, const T& __value) {
    for (;n > 0;--n, ++first)
        *first = __value;
    return first;
}

// Specialization: for one-byte types we can use memset.

inline void fill(unsigned char* first, unsigned char* last,
                 const unsigned char& c) {
  unsigned char tmp = c;
  memset(first, tmp, last - first);
}

inline void fill(signed char* first, signed char* last,
                 const signed char& c) {
  signed char tmp = c;
  memset(first, static_cast<unsigned char>(tmp), last - first);
}

inline void fill(char* first, char* last, const char& c) {
  char tmp = c;
  memset(first, static_cast<unsigned char>(tmp), last - first);
}

template <class Size>
inline unsigned char* fill_n(unsigned char* first, Size n,
                             const unsigned char& c) {
  fill(first, first + n, c);
  return first + n;
}

template <class Size>
inline signed char* fill_n(char* first, Size n,
                           const signed char& c) {
  fill(first, first + n, c);
  return first + n;
}

template <class Size>
inline char* fill_n(char* first, Size n, const char& c) {
  fill(first, first + n, c);
  return first + n;
}

//--------------------------------------------------
// equal and mismatch

template <class InputIt1, class InputIt2>
pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1,
                                    InputIt2 first2) {
    while (first1 != last1 && *first1 == *first2) {
        ++first1;
        ++first2;
    }
    return pair<InputIt1, InputIt2>(first1, first2);
}

template <class InputIt1, class InputIt2, class BiPredicate>
pair<InputIt1, InputIt2> mismatch(InputIt1 first1, InputIt1 last1,
                                    InputIt2 first2, BiPredicate pred) {
    while (first1 != last1 && pred(*first1, *first2)) {
        ++first1;
        ++first2;
    }
    return pair<InputIt1, InputIt2>(first1, first2);
}

template <class InputIt1, class InputIt2>
inline bool equal(InputIt1 first1, InputIt1 last1, InputIt2 first2) {
    for ( ; first1 != last1; ++first1, ++first2)
        if (*first1 != *first2)
        return false;
    return true;
}

template <class InputIt1, class InputIt2, class BiPredicate>
inline bool equal(InputIt1 first1, InputIt1 last1,
                  InputIt2 first2, BiPredicate pred) {
    for ( ; first1 != last1; ++first1, ++first2)
        if (!pred(*first1, *first2))
        return false;
    return true;
}

//--------------------------------------------------
// lexicographical_compare and lexicographical_compare_3way.

template <class InputIt1, class InputIt2>
bool lexicographical_compare(InputIt1 first1, InputIt1 last1,
                             InputIt2 first2, InputIt2 last2) {
    for (;first1 != last1 && first2 != last2;++first1, ++first2) {
        if (*first1 < *first2)
            return true;
        if (*first2 < *first1)
            return false;   
    }
    return first1 == last1 && first2 != last2;
}

template <class InputIt1, class InputIt2, class Compare>
bool lexicographical_compare(InputIt1 first1, InputIt1 last1,
                             InputIt2 first2, InputIt2 last2,
                             Compare comp) {
    for (;first1 != last1 && first2 != last2;++first1, ++first2) {
        if (comp(*first1, *first2))
            return true;    
        if (comp(*first2, *first1))
            return false;
    }
    return first1 == last1 && first2 != last2;
}

inline bool 
lexicographical_compare(const unsigned char* first1,
                        const unsigned char* last1,
                        const unsigned char* first2,
                        const unsigned char* last2) {
    const size_t len1 = last1 - first1;
    const size_t len2 = last2 - first2;
    const int res = memcmp(first1, first2, min(len1, len2));
    return res != 0 ? res < 0 : len1 < len2;
}


template <class InputIt1, class InputIt2>
int lexicographical_compare_3way(InputIt1 first1, InputIt1 last1,
                                   InputIt2 first2, InputIt2 last2) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2)
            return -1;
        if (*first2 < *first1)
            return 1;
        ++first1;
        ++first2;
    }
    if (first2 == last2) {
        return first1 == last1 ? 0 : 1;
    }
    else {
        return -1;
    }
}

inline int
lexicographical_compare_3way(const unsigned char* first1,
                             const unsigned char* last1,
                             const unsigned char* first2,
                             const unsigned char* last2) {
    const ptrdiff_t len1 = last1 - first1;
    const ptrdiff_t len2 = last2 - first2;
    const int res = memcmp(first1, first2, min(len1, len2));
    return res != 0 ? res : (len1 == len2 ? 0 : (len1 < len2 ? -1 : 1));
}

} // MiniSTL