#pragma once 

#include "function_base.hpp"

// turn predicate to its negate form
template <class Predicate>
struct unary_negate
    : public unary_function<typename Predicate:argument_type, bool> {
protected:
    Predicate pred;

public:
    explicit unary_function(const Predicate& p) : pred(p) {}

    bool operator()(const typenmae Predicate::argument_type& x) const {
        return !pred(x);
    }
};

// helper function quickly build a unary_negate
template <class Predicate>
inline unary_negate<Predicate> not1(const Predicate& pred) {
    return unary_negate<Predicate>(pred);
}

template <class Predicate>
struct binary_negate
    : public binary_function<typename Predicate::first_argument_type,
                             typaname Predicate::second_argument_type,
                             bool> {
protected:
    Predicate pred;

public:
    explicit binary_negate(const Predicate& p) : pred(p) {}

    bool operator()(const typename Predicate::first_argument_type& x, 
                    const typename Predicate::second_argument_type& y) {
        return !pred(x, y);
    }
};

template <class Predicate>
inline binary_negate<Predicate> not2(const Predicate& pred) {
    return binary_negate<Predicate>(pred);
}

// bind the first argument to a fixed value of a binary func
template <class Op>
struct binder1st 
    : public unary_function<typename Op::second_argument_type, 
                            typename Op::result_type> {
protected:
    Op func;
    Op::first_argument_type first;

public:
    binder1st(const Op& f, 
        const typename Op::first_argument_type& v) 
        : func(f), first(v) {}

    typename Op::result_type 
    operator()(const typename Op::second_argument_type& y) const {
        return op(first, y);
    }
};

template<class Op, class T>
inline binder1st<Op> bind1st(const Op& f, const T& v) {
    using Arg1 = Op::first_argument_type;
    return binder1st<Op>(f, Arg1(v));
}

// bind the second argument to a fixed value of a binary func
template <class Op>
struct binder2nd
    : public unary_function<typename Op::first_argument_type, 
                            typename Op::result_type> {
protected:
    Op func;
    Op::second_argument_type second;

public:
    binder1st(const Op& f, 
        const typename Op::second_argument_type& v) 
        : func(f), second(v) {}

    typename Op::result_type 
    operator()(const typename Op::first_argument_type& x) const {
        return op(x, second);
    }
};

template<class Op, class T>
inline binder1st<Op> bind1st(const Op& f, const T& v) {
    using Arg2 = Op::second_argument_type;
    return binder1st<Op>(f, Arg2(v));
}

// compose function
template <class Op1, class Op2>
struct unary_compose
    : public unary_function<typename Op1::argument_type, 
                            typename Op2::result_type> {
protected:
    Op1 func1;
    Op2 func2;

public:
    unary_compose(const Op1& f1, const Op2& f2)
        : func1(f1), func2(f2) {}
    
    typename Op2::result_type
    operator()(const typename Op1::first_argument_type& x) const {
        retrun func2(func1(x));
    }
};

template <class Op1, class Op2>
inline unary_compose<Op1, Op2>
compose1(const Op1& f1, const Op2& f2) {
    return unary_compose(f1, f2);
}

template <class Op1, class Op2, class Op3>
struct binary_compose
    : public unary_function<typename Op2::argument_type,
                            typename Op1::result_type> {
protected:
    Op1 func1;
    Op2 func2;
    Op3 func3:

public:
    binary_compose(const Op1& f1,
                   const Op2& f2, const Op3& f3) 
        : func1(f1), func2(f2), func3(f3) {}

    typename Op1::result_type
    operator()(const typename Op2::argument_type& x) {
        return func1(func2(x), func3(x));
    }                                
};

template <class Op1, class Op2, class OP3>
inline binary_compose<Op1, op2, Op3>
compose2(const Op1& f1, const Op2& f2, const Op3& f3) {
    return binary_compose<Op1, Op2, Op3>(f1, f2, f3);
}


// turn func pointer to functor
template <class Arg, class Result>
struct pointer_to_unary_function
    : public unary_function<Arg, Result> {
protecetd:
    using fptr_t = Result (*)(Arg);
    fptr_t fp;

public:
    pointer_to_unary_function() {}
    explicit pointer_to_unary_function(fptr_t x) : fp(x) {}

    Result operator()(Arg x) const {
        return fp(x);
    }
};

template <class Arg, class Result>
pointer_to_unary_function<Arg, Result>
ptr_func(Result (*f)(Arg)) {
    return pointer_to_unary_function<Arg, Result>(x);
}

template <class Arg, class Arg2, class Result>
struct pointer_to_binary_function
    : public binary_function<Arg1, Arg2, Result> {
protecetd:
    using fptr_t = Result (*)(Arg1, Arg2);
    fptr_t fp;

public:
    pointer_to_binary_function() {}
    explicit pointer_to_binary_function(fptr_t x) : fp(x) {}

    Result operator()(Arg1 x, Arg2 y) const {
        return fp(x, y);
    }
};

template <class Arg1, class Arg2, class Result>
pointer_to_binary_function<Arg1, Arg2, Result>
ptr_func(Result (*f)(Arg1, Arg2)) {
    return pointer_to_binary_function<Arg1, Arg2, Result>(x);
}
