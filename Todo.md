- [x] Allocator
    - [x] alloc.hpp: sgi-alloc, including malloc_alloc and default_alloc
    - [x] allocator.hpp: cpp-standard-conforming allocator ;  
                         allocAdaptor which turn sgi-alloc to std allocator ;  
                         alloc_traits encapsulates the difference among sgi/std/allocAdaptor
    - [x] little_Alloc.hpp: a little exercising alloc learning from <STL source code analysis>, not used
    - [x] construct.hpp: construct and destroy
    - [x] unintialized.hpp: uninitialized_copy(), unitialized fill()
    - [x] smart_pointer.hpp: RAII, auto_ptr, not used
    - [x] memory.hpp: include above 
  
- [x] Traits
    - [x] type_traits.hpp: extract features from type
  
- [x] Iterator
    - [x] iterator_base.hpp: iterator_traits ; func distance() and advance()
    - [x] iterator_adaptor.hpp: turn containter into insert_iterator ;
          input/output stream iterator

- [ ] Algorithms
    - [x] algobase.hpp
    - [x] heap.hpp
    - [x] algo_set.hpp
    - [x] sort.hpp
    - [x] numeric.hpp 
    - [x] algo_random.hpp 
    - [x] algorithms.hpp 

- [ ] Container
    - [x] Sequence
        - [x] vector.hpp
        - [x] list.hpp
        - [x] deque.hpp
        - [x] stack.hpp
        - [x] queue.hpp
        - [x] priority_queue.hpp
        - [x] forward_list.hpp
    - [ ] Associative
        - [x] rb_tree.hpp
        - [x] bs_tree.hpp
        - [ ] avl_tree.hpp
        - [x] set.hpp
        - [x] map_hpp
        - [x] multiset.hpp
        - [x] multimap.hpp
        - [ ] hashtable.hpp
        - [ ] hash_map.hpp
        - [ ] hash_set.hpp
        - [ ] hash_multimap.hpp
        - [ ] hash_multiset.hpp

- [x] Function:
    - [x] function_base.hpp
    - [x] function_adaptor.hpp
    - [x] function.hpp
  
 - [ ] Util
    - [x] pair.hpp: just like std::pair 
    - [x] tempbuf: temporary buffer for algorithms 
