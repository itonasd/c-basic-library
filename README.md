data structure library (for what?)


currently, only linked list is avaliable  
  circular linked list is not fully supported  
  working on double linked list traversal pointer(iterator) features (absolute index movement).  

  -concat (merge two linkedlist together) implemented.  
  -shift, unshift (delete node at tail) implemented.  
  
linked list initalization  
  linkedlist_t *name  = NULL;  
double linked list initalization  
  dlinkedlist_ptr *name = dlinkedlist_init(name, < int node location ...-2, -1, 0, 1, 2... >);  

iterator feature:  
only avaliable to double linked list  
only require once, to let iterator traverse to selected node. (O(index) time complexity)  
once selected node reached, iterator lets you delete (to head), insert (to head), retrieve node's value, write node's value (O(1) on all operation, excluding bulk writing) without traversing to the selected node again and again.  
on push or insert, iterator remains selecting selected node, in contrast, when selected node got either popped or deleted, iterator becomes invalid. free(<iterator_name>) to collapse iterator.  
multiple iterator can exit, each iterator operataing on different tasks in the same array as long as you are carefully dealing with them.  
