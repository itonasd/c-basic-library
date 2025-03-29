#include <string.h>
#include <stdlib.h>
#include "arr.h"

array filtered(array src, _callback_1arg callback) {
    if (!src || !callback) return 0;

    array dest = arr_init(src->size);
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
    if (!temp) {
        free(dsts);
        return 0;
    }

    dest->buffer = temp;

    return dest;
}

array mapped(array src, _callback_1arg callback) {
    if (!src || !callback) return 0;

    array dest = arr_init(src->size);
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

array reduced(array src, _callback_2arg callback, any initial) {
    if (!src || !callback) return 0;

    array dest = arr_init(src->size);
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

result findAll(array src, _callback_1arg callback) {
    if (!src || !callback) return 0;

    result ret = (result) malloc(sizeof(res));
    if (!ret) return 0;

    ret->length = src->length;
    ret->item = (items) malloc(sizeof(item) * src->length);
    if (!ret->item) {
        free(ret->item);
        free(ret);
        return 0;
    }

    uint64 size = src->size;
    uint64 retPointer = 0;
    raw srcs = src->buffer;

    for (uint64 i = 0; i < src->length; i++) {
        if (callback(srcs + (i * size))) {
            ret->item[retPointer].pointer = srcs + (i * size);

            ret->item[retPointer].value = (raw) malloc(size);
            if (!ret->item[retPointer].value) {
                for (uint64 i = 0; i < retPointer; i++)
                    free(ret->item[i].value);
                free(ret->item);
                free(ret);
                return 0;
            }

            memcpy(ret->item[retPointer].value, srcs + (i * size), size);

            ret->item[retPointer].index = i;
            retPointer++;
        }
    }

    if (!retPointer) {
        free(ret->item);
        ret->item = 0;
        ret->length = 0;
        return ret;
    }

    items new_item = (items) realloc(ret->item, sizeof(item) * retPointer);

    if (!new_item) {
        for (uint64 i = 0; i < retPointer; i++)
            free(ret->item[i].value);
        free(ret->item);
        free(ret);
        return 0;
    }

    ret->item = new_item;
    ret->length = retPointer;

    return ret;
}

items find(array src, _callback_1arg callback) {
    if (!src || !callback) return 0;
    items ret = (items) malloc(sizeof(item));

    uint64 size = src->size;
    raw srcs = src->buffer;

    for (uint64 i = 0; i < src->length; i++) {
        if (callback(srcs + (i * size))) {
            ret->pointer = srcs + (i * size);

            ret->value = (raw) malloc(size);
            if (!ret->value) {
                free(ret);
                return 0;
            }
            memcpy(ret->value, srcs + (i * size), size);

            ret->index = i;
            break;
        }
    }

    return ret;
}

array merged(array src_a, array src_b) {
    if (!src_a || !src_b || src_a->size != src_b->size) return 0;

    array dest = arr_init(src_a->size);
    if (!dest) return 0;

    dest->length = src_a->length + src_b->length;
    dest->alloc.length = src_a->length + src_b->length;
    dest->buffer = (raw) malloc(src_a->size * dest->length);

    if (!dest->buffer) {
        free(dest);
        return 0;
    }

    memcpy(
        dest->buffer, 
        src_a->buffer, 
        src_a->length * src_a->size
    );

    memcpy(
        dest->buffer + (src_a->length * src_a->size), 
        src_b->buffer, 
        src_b->length * src_b->size
    );

    return dest;
}

array reversed(array src) {
    if (!src) return 0;

    array dest = arr_init(src->size);
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
        memcpy(dsts + (i * size), srcs + ((length - 1 - i) * size), size);

    return dest;
}