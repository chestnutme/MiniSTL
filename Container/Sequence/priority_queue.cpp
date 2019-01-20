#pragma once

#include <initializer_list>
#include "vector.hpp"
#include "Function/function.hpp"

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
    priority_queue(const Compare& x, const Container&);
    explicit priority_queue(const Compare& x = Compare(), Container&& = Container());
    template <class InputIt>
    priority_queue(InputIt first, InputIt last,
                       const Compare& x, const Container&);
    template <class InputIt>
    priority_queue(InputIt first, InputIt last,
                   const Compare& x = Compare(), Container&& = Container());
    template <class Alloc> explicit priority_queue(const Alloc&);
    template <class Alloc> priority_queue(const Compare&, const Alloc&);
    template <class Alloc> priority_queue(const Compare&,
                                          const Container&, const Alloc&);
    template <class Alloc> priority_queue(const Compare&,
                                          Container&&, const Alloc&);
    template <class Alloc> priority_queue(const priority_queue&, const Alloc&);
    template <class Alloc> priority_queue(priority_queue&&, const Alloc&);
 
    bool empty() const {
        return c.empty();
    }
    size_type size() const {
        return c.size();
    }
    const_reference top() const {
        return c.front();
    }
    void push(const value_type& x);
    void push(value_type&& x);
    template <class... Args> void emplace(Args&&... args);
    void pop();
    void swap(priority_queue& q) noexcept( noexcept(swap(c, q.c))
                                        && noexcept(swap(comp, q.comp))) {
        using std::swap;
        swap(c, q.c);
        swap(comp, q.comp);
    }
};


} // MiniSTL