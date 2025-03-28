#include <string.h>
#include <stdlib.h>
#include "arr.h"

array filter(array src, _callback_1arg callback) {
    if (!src || !callback) return 0;

    array dest = init(src->size);
    if (!dest) return 0;
    
    dest->buffer = (raw) malloc(src->size * src->length);
    if (!dest->buffer) {
        free(dest);
        return 0;
    }

    raw srcs = src->buffer;
    raw dsts = dest->buffer;
    uint64 size = src->size;
    uint64 length = 0;

    for (uint64 i = 0; i < src->length; i++) {
        if ((raw) callback(srcs + (i * size))) {
            memcpy(dsts + (length * size), srcs + (i * size), size);
            length++;
        }
    }
    
    dest->length = length;
    dest->alloc.length = length;
    raw temp = (raw) realloc(dsts, length * size);
    if (!temp) return 0;
    dest->buffer = temp;

    return dest;
}

array map(array src, _callback_1arg callback) {
    if (!src || !callback) return 0;

    array dest = init(src->size);
    if (!dest) return 0;

    dest->length = src->length;
    dest->alloc.length = src->length;
    dest->buffer = (raw) malloc(src->size * src->length);
    if (!dest->buffer) {
        free(dest);
        return 0;
    }

    raw srcs = src->buffer;
    raw dsts = dest->buffer;
    uint64 size = src->size;
    uint64 length = src->length;

    for (uint64 i = 0; i < length; i++)
        memcpy(dsts + (i * size), callback(srcs + (i * size)), size);

    return dest;
}

array reduce(array src, _callback_2arg callback, any initial) {
    if (!src || !callback) return 0;

    array dest = init(src->size);
    if (!dest) return 0;

    dest->length = 1;
    dest->alloc.length = 1;
    dest->buffer = (raw) malloc(src->size);
    if (!dest->buffer) {
        free(dest);
        return 0;
    }

    raw dsts = dest->buffer;
    raw srcs = src->buffer;
    uint64 size = src->size;
    uint64 length = src->length;
    
    memcpy(dsts, initial, size);

    for (uint64 i = 0; i < length; i++)
        memcpy(dsts, callback(srcs + (i * size), dsts), size);
    
    return dest;
}

result find(array src, _callback_1arg callback) {
    result ret = (result) malloc(sizeof(res));
    ret->value = (raw*) malloc(src->size);
    ret->index = -1;
    ret->pointer = 0;
    memset(ret->value, 0xFF, src->size);
    if (!src || !callback) return ret;

    uint64 size = src->size;
    raw srcs = src->buffer;

    for (uint64 i = 0; i < src->length; i++) {
        if (callback(srcs + (i * size))) {
            ret->pointer = src->buffer + (i * size);
            memcpy(ret->value, src->buffer + (i * size), size);
            ret->index = i;
            break;
        }
    }

    return ret;
}