#ifndef _ARR_H_
#define _ARR_H_

#ifdef ARRAY_H
#error "INCOMPATIBLE"
#endif

#if defined(__GNUC__)
#if __SIZEOF_POINTER__ != 8
#error "AMD64"
#endif
#endif

typedef unsigned long long uint64;
typedef unsigned char uint8;
typedef long long int64;
typedef char int8;

typedef void* callback;
typedef void* any;
typedef unsigned char* raw;
typedef callback (*_callback_1arg)(any);
typedef callback (*_callback_2arg)(any, any);

// default macros

#define true (void*)1
#define false (void*)0

#define to(type, value) (*(type*)(value))
#define boolean(condition) ((condition) ? true : false)
#define new(type) arr_init(sizeof(type))
#define get(type, src, index) (to(type, gather(src, index)))
#define get2D(type, src, i, j) get(int, deref(src, i), j)
#define get3D(type, src, i, j, k) get(int, deref(deref(src, i), j), k)

#define push(type, dest, value) do {                \
    type a = value;                                 \
    write(dest, dest->length, dest->length, &a);    \
} while (0)

#define shift(type, dest, value) do {               \
    type a = value;                                 \
    insert(dest, 0, 0, &a);                         \
} while (0)

#define pop(dest) do {                              \
    erase(dest, dest->length - 1, dest->length - 1);\
} while (0)

#define unshift(dest) do {                          \
    erase(dest, 0, 0);                              \
} while (0)

#define start() 0
#define end(dest) do {                              \
    dest->length - 1;                               \
} while (0)

// attribute macros

#define prefetchl1(addr) asm volatile("prefetcht0 %0" :: "m"(*(const char *)(addr)))
#define prefetchl2(addr) asm volatile("prefetcht1 %0" :: "m"(*(const char *)(addr)))
#define prefetchl3(addr) asm volatile("prefetcht2 %0" :: "m"(*(const char *)(addr)))

// ranges macros

#define foreach(type, var, src)                     \
for (uint64 __iterator = 0; __iterator < src->length; __iterator++) { \
    type var = to(type, src->buffer + (__iterator * src->size));

#define cycle(type, var, src)                       \
for (uint64 __iterator = 0;; __iterator = (__iterator + 1) % src->length) { \
    type var = to(type, src->buffer + (__iterator * src->size));

#define endstate }

// performance macros

#define benchmark                                   \
do {                                                \
    struct timespec __start_time, __end_time;       \
    clock_gettime(CLOCK_MONOTONIC, &__start_time);  \

#define endbench                                    \
    clock_gettime(CLOCK_MONOTONIC, &__end_time);    \
    double __elapsed_time =                         \
        (__end_time.tv_sec - __start_time.tv_sec) + \
        (__end_time.tv_nsec - __start_time.tv_nsec) / 1e9; \
    printf("elapsed time: %.12lf seconds\n", __elapsed_time - 0.0000001); \
} while (0);

#define rdtsc                                       \
do {                                                \
    unsigned long long __start_cycles;              \
    asm volatile ("rdtsc" : "=A" (__start_cycles));

#define endtsc                                      \
    unsigned long long __end_cycles;                \
    asm volatile ("rdtsc" : "=A" (__end_cycles));   \
    printf("cycles elapsed: %llu\n", __end_cycles - __start_cycles); \
} while (0);

#ifdef __linux__

#define malinfo do {                               \
    uint64 __st_mmory = mallinfo2().uordblks;       \

#define endmal                                     \
    printf("memory allocated: %llu bytes\n", mallinfo2().uordblks - __st_mmory); \
} while (0);

#endif

#define __TYPE_NAME__(type) _Generic((type){0}, \
        int: "int", \
        unsigned int: "unsigned int", \
        long: "long", \
        unsigned long: "unsigned long", \
        char: "char", \
        unsigned char: "unsigned char", \
        short: "short", \
        unsigned short: "unsigned short", \
        float: "float", \
        double: "double", \
        default: "unknown" \
)

#define clog_debug(type, src) do { \
    raw _byte = src->buffer; \
    type *_val = (type *)src->buffer; \
    const char *_formatting = _Generic((type){0}, \
        int: "] %d\n", \
        unsigned int: "] %u\n", \
        long: "] %ld\n", \
        unsigned long: "] %lu\n", \
        char: "] %c\n", \
        unsigned char: "] %hhu\n", \
        short: "] %hd\n", \
        unsigned short: "] %hu\n", \
        float: "] %.2f\n", \
        double: "] %.4lf\n", \
        default: "] unknown\n" \
    ); \
    \
    printf("\n    clog_debug %s %s\n", __TYPE_NAME__(type), #src); \
    printf("\n    initialized region\n"); \
    \
    uint64 _iter; \
    for (uint64 _iter = 0; _iter < src->length; _iter++) { \
        printf("  %5llu: [ ", _iter); \
        \
        for (uint64 _innerIter = 0; _innerIter < src->size; _innerIter++) \
            printf("0x%02x ", _byte[_iter * src->size + _innerIter]); \
        printf(_formatting, _val[_iter]); \
    } \
    \
    printf("    allocated region\n"); \
    if (src->length == src->alloc.length) printf("     none\n"); \
    for (_iter = src->length; _iter < src->alloc.length; _iter++) { \
        printf("  %5llu: [ ", _iter); \
        \
        for (uint64 _innerIter = 0; _innerIter < src->size; _innerIter++) \
            printf("0x%02x ", _byte[_iter * src->size + _innerIter]); \
        printf(_formatting, _val[_iter]); \
    } \
    \
    printf("\n    statistic\n"); \
    printf("     size: %llu, length: %llu\n", src->size, src->length); \
    printf("     capacity: %llu, factor: %llu\n", src->alloc.length, src->alloc.factor); \
    printf("     struct addrs: %p\n", src); \
    printf("     buffst addrs: %p\n", src->buffer); \
    printf("     buffen addrs: %p\n", src->buffer + src->alloc.length); \
    printf("     buffer index: %d - %llu\n\n", 0, (src->size * src->alloc.length) - 1); \
} while (0)

#define clog_array(type, src) do { \
    type *_val = (type *)src->buffer; \
    const char *_formatting = _Generic((type){0}, \
        int: " %d", \
        unsigned int: " %u", \
        long: " %ld", \
        unsigned long: " %lu", \
        char: " %c", \
        unsigned char: " %hhu", \
        short: " %hd", \
        unsigned short: " %hu", \
        float: " %.2f", \
        double: " %.4lf", \
        default: " unknown" \
    ); \
    \
    printf("\n    clog_array %s %s", __TYPE_NAME__(type), #src); \
    printf("\n     ["); \
    for (uint64 i = 0; i < src->length; i++) { \
        printf(_formatting, _val[i]); \
        if (i < src->length - 1)printf(","); \
    } \
    printf(" ]\n"); \
} while (0)

#define clog_array2D(type, src) do { \
    const char *_formatting = _Generic((type){0}, \
        int: " %d", \
        unsigned int: " %u", \
        long: " %ld", \
        unsigned long: " %lu", \
        char: " %c", \
        unsigned char: " %hhu", \
        short: " %hd", \
        unsigned short: " %hu", \
        float: " %.2f", \
        double: " %.4lf", \
        default: " unknown" \
    ); \
    \
    printf("\n    clog_array2D %s %s\n", __TYPE_NAME__(type), #src); \
    printf("     [\n"); \
    for (uint64 i = 0; i < src->length; i++) { \
        type *_val = (type *) deref(src, i)->buffer; \
        printf("      ["); \
        for (uint64 j = 0; j < deref(src, i)->length; j++) { \
            printf(_formatting, _val[j]);\
            if (j < deref(src, i)->length - 1) printf(","); \
        } \
        printf(" ]"); \
        if (i < src->length - 1)printf(","); \
        printf("\n"); \
    } \
    printf("     ]\n"); \
} while (0)

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

// 24ULL
typedef struct _item {
    raw pointer;
    any value;
    uint64 index;
} item;

typedef item* items;

// 16ULL
typedef struct _res {
    items item;
    uint64 length;
} res;

typedef arr* array;
typedef res* result;

array arr_init(uint64 size);

void arr_config(array dest, uint64 size);

void arr_free(array src);

void item_free(items src);

void res_free(result src);

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

result findAll(array src, _callback_1arg callback);
items find(array src, _callback_1arg callback);

any gather(array src, uint64 index);

array deref(array src, uint64 index);

array extract(array src, uint64 start, uint64 end);

array write(array dest, uint64 start, uint64 end, any src);

array insert(array dest, uint64 start, uint64 end, any src);

array fill(array dest, uint64 start, uint64 end, any val);

array erase(array dest, uint64 start, uint64 end);

array flush(array dest, double percentage);

#ifdef SORT_UNAVALIABLE

#define descending(type) _##type##_descending_sorting_template
#define ascending(type) _##type##_ascending_sorting_template

#else
#define SORT_IS_NOT_INCLUDED
#endif
#endif