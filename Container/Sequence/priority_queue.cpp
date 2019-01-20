#pragma once

#include <initializer_list>
#include <utility>
#include "vector.hpp"
#include "Function/function.hpp"
#include "Algorithms/heap.hpp"


namespace MiniSTL {

// Container must support random access
template <class T,
          class Container = vector<T>,
          class Compare = less<typename Container::value_type> >
class priority_queue {
public:

    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using size_type = typename Container::size_type;
    using container_type = Container;

protected:
    Container c;
    Compare comp;

public:
    // ctor
    priority_queue(const Compare& x, const Container& y) : c(y), comp(x) {
        make_heap(c.begin(), c.end(), comp);
    }

    explicit priority_queue(const Compare& x = Compare(), Container&& y = Container()) 
        : c(std::forward(y)), comp(c) {
        make_heap(c.begin, c.end(), comp);
    }

    template <class InputIt>
    priority_queue(InputIt first, InputIt last,
                   const Compare& x, const Container& y) : c(y), comp(x) { 
        c.insert(first, last);
        make_heap(c.begin, c.end(), comp);
    }

    template <class InputIt>
    priority_queue(InputIt first, InputIt last,
                   const Compare& x = Compare(), Container&& = Container()) 
        : c(std::forward(y)), comp(c) {
        c.insert(first, last);
        make_heap(c.begin, c.end(), comp);
    }

    priority_queue() : c() {}
    priority_queue(const priority_queue& q) : c(q.c), comp(q.comp) {}
    priority_queue(priority_queue&& q)
        : c(std::move(q.c)), comp(std::move(q.comp)) {}

    // assign
    priority_queue& operator=(const priority_queue& q) {
        if(&q != this) {
            c = q.c;
            comp = q.comp;
        }
        return *this;
    }
    priority_queue& operator=(priority_queue&& q) {
        if(&q != this) {
            c = std::move(q.c);
            comp = std::move(q.comp);
        }
        return *this;
    }

    // capacity
    bool empty() const {
        return c.empty();
    }
    size_type size() const {
        return c.size();
    }

    // element access
    const_reference top() const {
        return c.front();
    }

    // modifiers:
    void push(const value_type& x) {
        c.push_back(x);
        push_heap(c.begin(), c.end(), comp);
    }
    void push(value_type&& x) {
        c.push_back(std::forward(x));
        push_heap(c.begin(), c.end(), comp);
    }

    template <class... Args> 
    void emplace(Args&&... args) {
        c.emplace_back(args...);
        push_heap(c.begin(), c.end(), comp);
    }

    void pop() {
        pop_heap(c.begin(), c.end(), comp);
        c.pop_back();
    }

    void swap(priority_queue& q) noexcept( noexcept(swap(c, q.c))
                                        && noexcept(swap(comp, q.comp))) {
        MiniSTL::swap(c, q.c);
        MinisTL::swap(comp, q.comp);
    }
};


} // MiniSTL