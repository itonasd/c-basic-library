#include <string.h>
#include <stdlib.h>
#include "arr.h"

array filter(array dest, _callback_1arg callback) {
    if (!dest || !callback) return 0;

    raw extract = (raw) malloc(dest->size * dest->length);
    if (!extract) return 0;

    raw dsts = dest->buffer;
    uint64 size = dest->size;
    uint64 length = 0;

    for (uint64 i = 0; i < dest->length; i++) {
        if ((raw) callback(dsts + (i * size))) {
            memcpy(
                extract + (length * size),
                dsts + (i * size),
                size
            );  
            length++;
        }
    }

    memcpy(
        dsts,
        extract,
        length * size
    );

    memset(
        dsts + (length * size),
        0xFF,
        (dest->length - length) * size
    );

    dest->length -= dest->length - length;
    dest->alloc.length += dest->length - length;

    return dest;
}

array map(array dest, _callback_1arg callback) {
    if (!dest || !callback) return 0;

    raw dsts = dest->buffer;
    uint64 size = dest->size;

    for (uint64 i = 0; i < dest->length; i++)
        memcpy(
            dsts + (i * size), 
            callback(dsts + (i * size)), 
            size
        );

    return dest;
}

array reduce(array dest, _callback_2arg callback, any initial) {
    if (!dest || !callback) return 0;

    raw dsts = dest->buffer;
    uint64 size = dest->size;

    unsigned char extract[size];
    memcpy(extract, initial, size);

    for (uint64 i = 0; i < dest->length; i++)
        memcpy(
            extract, 
            callback(dsts + (i * size), extract), 
            size
        );

    memset(
        dsts + (1 * size), 
        0xFF, 
        (dest->length - 1) * size
    );

    memcpy(dsts, extract, size);
    dest->length = 1;

    return dest;
}

array merge(array dest, array src) {
    if (!dest || !src || dest->size != src->size) return 0;

    raw dsts = dest->buffer;
    raw srcs = src->buffer;
    uint64 size = dest->size;

    if (dest->length + src->length > dest->alloc.length) {
        uint64 new_alloc_length = dest->length + src->length + dest->alloc.factor;
        raw new_dsts = (raw) realloc(dsts, new_alloc_length * size);
        if (!new_dsts) return 0;

        dest->buffer = new_dsts;
        dsts = new_dsts;
        memset(
            dsts + (dest->alloc.length * size),
            0xFF,
            (new_alloc_length - dest->alloc.length) * size
        );
        dest->alloc.length = new_alloc_length;
    }

    memcpy(
        dsts + (dest->length * size),
        srcs,
        src->length * size
    );
    dest->length += src->length;

    return dest;
}

array reverse(array dest) {
    if (!dest) return 0;

    uint64 size = dest->size;
    raw dsts = dest->buffer;

    for (uint64 i = 0, j = dest->length - 1; i < j; i++, j--) {
        unsigned char temp[size];
        memcpy(temp, dsts + (i * size), size);
        memcpy(dsts + (i * size), dsts + (j * size), size);
        memcpy(dsts + (j * size), temp, size);
    }

    return dest;
}