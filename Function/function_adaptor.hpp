#pragma once 

#include "function_base.hpp"

namespace MiniSTL {


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

// Adaptor function: pointer to member function
// There are a total of 8 = 2^3 function objects in this family.
//      1. Member func taking no arguments vs taking one argument
//      2. Call throught pointer vs call through reference
//      3. Const vs non-const member function
// helper function mem_fun and mem_fun_ref can build the above
// function adaptor quickly.
// pointer to member function usually is declared as follows:
// class T {
//      int f() { return 7; }
//      int (A::*pf)();
//      A() : pf(&T::f);
//}

// no argument, through pointer
template <class Ret, class T>
class mem_fun_t : public unary_function<T*, Ret> {
private:
    Ret (T::*func)();

public:
    explicit mem_fun_t(Ret (T::*f)()) : func(f) {}
    Ret operator()(T* t) const { return (t->*f)(); }
};

template <class Ret, class T>
inline mem_fun_t<Ret, T> mem_fun(Ret (T::*f)()) {
    return mem_fun_t<Ret, T>(f);
}

// no argument, through const pointer
template <class Ret, class T>
class const_mem_fun_t : public unary_function<const T*, Ret> {
private:
    Ret (T::*func)() const;

public:
    explicit const_mem_fun_t(Ret (T::*f)() const) : func(f) {}
    Ret operator()(const T* t) const { return (t->*f)(); }
};

template <class Ret, class T>
inline const_mem_fun_t<Ret, T> mem_fun(Ret (T::*f)() const) {
    return const_mem_fun_t<Ret, T>(f);
}

// no argument, through ref
template <class Ret, class T>
class mem_fun_ref_t : public unary_function<T, Ret> {
private:
    Ret (T::*func)();

public:
    explicit mem_fun_t(Ret (T::*f)()) : func(f) {}
    Ret operator()(T& t) const { return (t.*f)(); }
};

template <class Ret, class T>
inline mem_fun_ref_t<Ret, T> mem_fun_ref(Ret (T::*f)()) {
    return mem_fun_ref_t<Ret, T>(f);
}

// no argument, through const ref
template <class Ret, class T>
class const_mem_fun_ref_t : public unary_function<T, Ret> {
private:
    Ret (T::*func)() const;

public:
    explicit const_mem_fun_ref_t(Ret (T::*f)() const) 
        : func(f) {}
    Ret operator()(const T& t) const { return (t.*f)(); }
};

template <class Ret, class T>
inline const_mem_fun_ref_t<Ret, T> mem_fun_ref(Ret (T::*f)() const) {
    return const_mem_fun_ref_t<Ret, T>(f);
}

// one argument, through pointer
template<class Ret, class T, class Arg>
class mem_fun1_t : public binary_function<T*, Arg, Ret> {
private:
    Ret (T::*func)(Arg);

public:
    explicit mem_fun1_t(Ret (T::*f)(Arg)) : func(f) {}
    Ret operator()(T* t, Arg x) const {
        return (t->*func)(x);
    }
};

template<class Ret, class T, class Arg>
mem_fun1_t<Ret, T, Arg> mem_fun1(Ret (T::*f)(Arg)) {
    return mem_fun1_t<Ret, T, Arg>(f);
}

// one argument, through const pointer
template<class Ret, class T, class Arg>
class const_mem_fun1_t : public binary_function<const T*, Arg, Ret> {
private:
    Ret (T::*func)(Arg) const;

public:
    explicit const_mem_fun1_t(Ret (T::*f)(Arg) const) 
        : func(f) {}
    Ret operator()(const T* t, Arg x) const {
        return (t->*func)(x);
    }
};

template<class Ret, class T, class Arg>
const_mem_fun1_t<Ret, T, Arg> mem_fun1(Ret (T::*f)(Arg) const) {
    return const_mem_fun1_t<Ret, T, Arg>(f);
}

// one argument, through ref
template<class Ret, class T, class Arg>
class mem_fun1_ref_t : public binary_function<T, Arg, Ret> {
private:
    Ret (T::*func)(Arg);

public:
    explicit mem_fun1_ref_t(Ret (T::*f)(Arg)) : func(f) {}
    Ret operator()(T& t, Arg x) const {
        return (t.*func)(x);
    }
};

template<class Ret, class T, class Arg>
mem_fun1_ref_t<Ret, T, Arg> mem_fun1(Ret (T::*f)(Arg)) {
    return mem_fun1_ref_t<Ret, T, Arg>(f);
}

// one argument, through const ref
template<class Ret, class T, class Arg>
class const_mem_fun1_ref_t : public binary_function<T*, Arg, Ret> {
private:
    Ret (T::*func)(Arg) const;

public:
    explicit const_rmem_fun1_ref_t(Ret (T::*f)(Arg) const) 
        : func(f) {}
    Ret operato()(const T& t, Arg x) const {
        return (t.*func)(x);
    }
};

template<class Ret, class T, class Arg>
const_mem_fun1_ref_t<Ret, T, Arg> 
mem_fun1(Ret (T::*f)(Arg) const) {
    return const_mem_fun1_ref_t<Ret, T, Arg>(f);
}

} // MiniSTL
