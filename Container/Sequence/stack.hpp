#pragma 

#include "deque.hpp"

namespace MiniSTL {

template <class T, class Container = deque<T> >
class stack {
public:
    using value_type = typename Container::value_type;
    using reference = typename Container::reference;
    using const_reference = typename Container::const_reference;
    using size_type = typename Container::size_type;
    using container_type = Container;
 
protected:
    Container c;
 
public:
    explicit stack(const Container& c);
    explicit stack(Container&& c = Container());
    stack(const stack& s);
    stack(stack&& s);
 
    bool empty() const {
        return c.empty();
    }
    size_type size() const {
        return c.size();
    }
    reference top() {
        return c.back();
    }
    const_reference top() const {
        return c.back();
    }
    void push(const value_type& x) {
        c.push_back(x);
    }
    void push(value_type&& x) {
        c.push_back(std::move(x));
    }
    template <class... Args> void emplace(Args&&... args) {
        c.emplace_back(std::forward<Args>(args)...);
    }
    void pop() {
        c.pop_back();
    }
    void swap(stack& s) noexcept(noexcept(swap(c, s.c))) {
        MiniSTL::swap(c, s.c);
    }
};

template <class T, class Container = deque<T> > class stack;
 
template <class T, class Container>
bool operator==(const stack<T, Container>& x,const stack<T, Container>& y);
template <class T, class Container>
bool operator!=(const stack<T, Container>& x,const stack<T, Container>& y);

template <class T, class Container>
bool operator< (const stack<T, Container>& x,const stack<T, Container>& y);
template <class T, class Container>
bool operator> (const stack<T, Container>& x,const stack<T, Container>& y);
template <class T, class Container>
bool operator>=(const stack<T, Container>& x,const stack<T, Container>& y);
template <class T, class Container>
bool operator<=(const stack<T, Container>& x,const stack<T, Container>& y);

template <class T, class Container>
void swap(stack<T, Container>& x, stack<T, Container>& y)
        noexcept(noexcept(x.swap(y))) {
    x.swap(y);
}

} // MiniSTL
