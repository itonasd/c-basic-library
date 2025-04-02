8 bytes arrays are very powerful lol  
it literally can store any data type    
including itself, any structs   

## Nested Array Guide
~~~
    
    array l3 = new(array);            |  create new array (size: 8)
    array l2 = new(array);            |  create new array (size: 8)
    array stuff = new(int);           |  create new array (size: 4)
                                      |
    push(int, stuff, 69420);          |  pushes 69420 into stuff array
    push(array, l2, stuff);           |  pushes stuff array into l2 array
    push(array, l3, l2);              |  pushes l2 array array into l3 array
                                      |
    arr_log(deref(deref(l3, 0), 0));  |  dereference from l3 array output: 69420    
    
~~~
Note: it copies references, not the actual data.