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

void config(array dest, uint64 size);

array init(uint64 size);

array filter(array src, _callback_1arg callback);

array map(array src, _callback_1arg callback);

array reduce(array src, _callback_2arg callback, any initial);

array reverse(array src);

result find(array src, _callback_1arg callback);

void write(array dest, uint64 start, uint64 end, any src);

void insert(array dest, uint64 start, uint64 end, any src);

void erase(array dest, uint64 start, uint64 end);

void flush(array dest, double percentage);

#ifdef SORT_UNAVALIABLE

#define descending(type) _##type##_descending_sorting_template
#define ascending(type) _##type##_ascending_sorting_template

#else
#define SORT_IS_NOT_INCLUDED
#endif
#endif

/*

void log(arr *buffer) {
    raw byte = (raw)buffer->buffer;
    int *_int = (int *)buffer->buffer;
    
    printf("\n    ARRAY LOG\n\n");
    printf("    initialized region\n");
    for (int i = 0; i < buffer->length; i++) {
        printf("    cell %2d: [ ", i);
        for (int j = 0; j < buffer->size; j++) {
            printf("0x%02x ", byte[i * buffer->size + j]);
        }
        printf("]  %d\n", _int[i]);
    }
    printf("\n    pre allocated region\n");
    if (buffer->alloc.length == buffer->length) printf("    none\n");
    for (int i = buffer->length; i < buffer->alloc.length; i++) {
        printf("    cell %2d: [ ", i);
        for (int j = 0; j < buffer->size; j++) {
            printf("0x%02x ", byte[i * buffer->size + j]);
        }
        printf("]  %d\n", _int[i]);
    }

    printf("\n");
    printf("    size: %ld, length: %ld, total: %ld, segment: ", buffer->size, buffer->length, buffer->alloc.length);
    printf("\n");
    printf("    buffer start address : %p : %d\n", buffer->buffer, 0);
    printf("    buffer end address   : %p : %ld\n\n", buffer->buffer + (buffer->size * buffer->alloc.length) - 1, (buffer->size * buffer->alloc.length) - 1);
}
    
*/