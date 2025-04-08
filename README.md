an array, once created, if it fits, it can be stored    
type unsafe    

## Nested Array Guide
~~~
    
    array l3 = new(array);                    |  create new array (size: 8)
    array l2 = new(array);                    |  create new array (size: 8)
    array stuff = new(int);                   |  create new array (size: 4)
                                              |
    push(int, stuff, 69420);                  |  pushes 69420 into stuff array
    push(array, l2, stuff);                   |  pushes stuff array into l2 array
    push(array, l3, l2);                      |  pushes l2 array array into l3 array
                                              |
    clog_array(int, deref(deref(l3, 0), 0));  |  dereference from l3 array and casts to int, output: 69420    
    
~~~
Note: it copies references, not the actual data.    
macros does not compatible with stack array.    
