1. what problems caused by dynamic alloc? and how default_alloc fix them?

2. move semantics of container? after move, container should be 
        a. no elements but empty initialized 
        b. no elements and inside state is null

3. what will happen when destroy unitialized element？