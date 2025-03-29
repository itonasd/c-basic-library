#ifndef ARR_H
#define ARR_H

#ifdef ARRAY_H
#error "INCOMPATIBLE"
#endif

#ifndef __INT_T
#define __INT_T
#if defined(__GNUC__)
#if __SIZEOF_POINTER__ == 8
typedef unsigned long long uint64;
typedef unsigned char uint8;
typedef long long int64;
typedef char int8;
#else
#error "AMD64"
#endif
#endif
#endif

typedef void* callback;
typedef void* any;
typedef unsigned char* raw;
typedef callback (*_callback_1arg)(any);
typedef callback (*_callback_2arg)(any, any);

#define to(type, value) (*(type*)(value))
#define true (void*)1
#define false (void*)0

// 40ULL
typedef struct _arr {
    raw buffer;
    uint64 length;
    uint64 size;

    struct _mem {
        uint64 length;
        uint64 factor;
    } alloc;

} arr; 

typedef struct _res {
    raw pointer;
    any value;
    uint64 index;
} res;

typedef arr* array;
typedef res* result;

void arr_config(array dest, uint64 size);

array arr_init(uint64 size);


array filtered(array src, _callback_1arg callback);
array filter(array dest, _callback_1arg callback);

array mapped(array src, _callback_1arg callback);
array map(array dest, _callback_1arg callback);

array reduced(array src, _callback_2arg callback, any initial);
array reduce(array dest, _callback_2arg callback, any initial);

array merged(array src_a, array src_b);
array merge(array dest, array src);

array reversed(array src);
array reverse(array dest);


result find(array src, _callback_1arg callback);

array write(array dest, uint64 start, uint64 end, any src);

array insert(array dest, uint64 start, uint64 end, any src);

array erase(array dest, uint64 start, uint64 end);

array flush(array dest, double percentage);

void arr_log(array src);

void arr_free(array src);

#ifdef SORT_UNAVALIABLE

#define descending(type) _##type##_descending_sorting_template
#define ascending(type) _##type##_ascending_sorting_template

#else
#define SORT_IS_NOT_INCLUDED
#endif
#endif