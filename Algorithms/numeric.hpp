#pragma once 

#include "Function/function_base.hpp"
#include "Iterator/iterator_base.hpp"

namespace MiniSTL {

template <class InputIt, class T, class BiOp = plus<T> >
T accumulate(InputIt first, InputIt last, T init = T(0), BiOp op = BiOp()) {
    while(first != last)
        init = op(init, *first++);
    return init;
}

template <class InputIt1, class InputIt2, class T,
          class BiOp1 = plus<T>, class BiOp2 = multiplies<T> >
T inner_product(InputIt1 first1, InputIt1 last1, 
                InputIt2 first2, T init = T(0), 
                BiOp1 op1 = BiOp1(), BiOp2 op2 = BiOp2()) {
    while(first1 != last1)
        init = op1(init, op1(*first1++, *first2++));
    return init;
}

template <class InputIt, class OutputIt, 
          class BiOp = plus<value_type_t<InputIt>> >
OutputIt
partial_sum(InputIt first, InputIt last, OutputIt result,
            BiOp op = BiOp()) {
    if(first == last)
        return result;
    *result = value_type_t<InputIt>(0);
    while(first != last)
        *++result = op(*result, *first++);
    return result;
}


template <class InputIt, class OutputIt, 
          class BiOp = minus<value_type_t<InputIt>> >
OutputIt
adjacent_difference(InputIt first, InputIt last, OutputIt result,
                    BiOp op = BiOp()) {
    if(first == last)
        return result;
    *result = *first;
    value_type_t<InputIt> tmp = *first;
    while(++first != last) {
        value_type_t<InputIt> v = *first;
        *++result = op(v, tmp);
        tmp = v;
    }
    return ++result;
}

// quick power algo
// power number x = (1 * 2**m + 0 * 2**(m-1) + ...) * 2**k
// eg: 13 ** 20 = 13 ** ((1 * 2**2 + 0 * 2**1 + 1 * 2**0) * 2**2)
//              = ((13 ** 2) ** 2) * (1 * 2**2 + 1 * 2**0)
//              = ((13 ** 2) ** 2) * ((((13 ** 2) ** 2) ** 2) **2)
template <class T, class Integer, class MonoidOp = multiplies<T>>
inline T power(T x, Integer n , MonoidOp op = MonoidOp()) {
    if(n == 0) {
        return identity_element(op);
    } else {
        while((n & 1) == 0) {
            x = op(x, x);
            n >>= 1;
        }

        T result = x;
        n >>= 1;
        while(n != 0) {
            x = op(x, x);
            if((n & 1) != 0)
                result = op(result, x);
            n >> 1;
        }
        return result;
    }
}

template <class ForwardIt, class T>
void iota(ForwardIt first, ForwardIt last, T init = T(0)) {
    while(first != last) 
        *fisrt++ = init++;
}


} // MiniSTL