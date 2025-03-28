#include <stdlib.h>
#include <string.h>
#include "arr.h"

void config(array dest, uint64 size) {
    dest->buffer = 0x0;
    dest->length = 0;
    dest->size = size;
    dest->alloc.length = 0;
    dest->alloc.factor = 0;
}

array init(uint64 size) {
    array dest = (array) malloc(sizeof(arr));
    config(dest, size);
    return dest;
}

void write(
    array dest, 
    uint64 start, 
    uint64 end, 
    any src
) {
    if (!dest || !src) return;
    uint8 swap = 0;
    if (start > end) {
        swap = 1;
        start ^= end;
        end ^= start;
        start ^= end;
    }

    uint64 size = dest->size;
    uint64 alloc_length = dest->alloc.length;
    uint64 alloc_fac = dest->alloc.factor;
    raw dst = dest->buffer;

    if (dst == 0 || end >= alloc_length) {
        uint64 new_alloc_length = end + 1 + alloc_fac;
        raw new_dst = (raw) realloc(dst, new_alloc_length * size);
        if (!new_dst) return;

        dest->buffer = new_dst;
        dst = new_dst;
        memset(dst + alloc_length * size, 0xFF, (new_alloc_length - alloc_length) * size);
        dest->alloc.length = new_alloc_length;
    }

    if (swap)
        for (uint64 i = 0; i < (end - start + 1); i++)
            memcpy(
                dst + (start + i) * size,
                src + (end - start - i) * size,
                size
            );
    else
        memcpy(
            dst + start * size,
            src,
            (end - start + 1) * size
        );
    
    dest->length = (dest->length > end + 1) ? dest->length : end + 1;
}

void insert(
    array dest, 
    uint64 start, 
    uint64 end, 
    any src
) {
    if (!dest || !src) return;

    if (start >= dest->length) {
        write(dest, start, end, src);
        return;
    }

    uint8 swap = 0;
    if (start > end) {
        swap = 1;
        start ^= end;
        end ^= start;
        start ^= end;
    }

    uint64 size = dest->size;
    uint64 length = dest->length;
    uint64 alloc_length = dest->alloc.length;
    uint64 alloc_fac = dest->alloc.factor;
    raw dst = dest->buffer;
    
    dest->length += ((end + 1) - start);
    if (dest->length > alloc_length) {
        uint64 new_alloc_length = dest->length + alloc_fac;
        raw new_dst = (raw) realloc(dst, new_alloc_length * size);
        if (!new_dst) return;

        dest->buffer = new_dst;
        dst = new_dst;
        memset(dst + alloc_length * size, 0xFF, (new_alloc_length - alloc_length) * size);
        dest->alloc.length = new_alloc_length;
    }

    memmove(
        dst + ((end + 1) * size), 
        dst + (start * size), 
        (length - start) * size
    );

    if (swap)
        for (uint64 i = 0; i < ((end + 1) - start); i++)
            memcpy(
                dst + (start + i) * size,
                src + (end - start - i) * size,
                size
            );
    else
        memcpy(
            dst + (start * size), 
            src, 
            ((end + 1) - start) * size
        );
}

void erase(array dest, uint64 start, uint64 end) {
    if (!dest || start >= dest->length || end >= dest->length) return;
    
    if (start > end) {
        start ^= end;
        end ^= start;
        start ^= end;
    }

    uint64 size = dest->size, length = dest->length;
    raw dsts = dest->buffer;

    memmove(
        dsts + (start * size), 
        dsts + ((end + 1) * size), 
        (length - (end + 1)) * size
    );

    memset(
        dsts + ((length - ((end + 1) - start)) * size), 
        0xFF, 
        ((end + 1) - start) * size
    );

    dest->length -= ((end + 1) - start);
}

void flush(array dest, double percentage) {
    if (!dest) return;
    uint64 old_length = dest->alloc.length;
    uint64 size = dest->size;
    dest->alloc.length -= ((dest->alloc.length - dest->length) * (percentage / 100));

    if (dest->alloc.length != old_length) {
        raw new_buffer = (raw) realloc(dest->buffer, dest->alloc.length * size);
        if (!new_buffer) return;
        dest->buffer = new_buffer;

        if (dest->alloc.length > old_length)
            memset(
                dest->buffer + (old_length * size), 
                0xFF, 
                (dest->alloc.length - old_length) * size
            );

        if (dest->alloc.length < dest->length) {
            memset(
                dest->buffer + (dest->alloc.length * size),
                0xFF,
                (dest->length - dest->alloc.length) * size
            );

            dest->length -= (dest->length - dest->alloc.length);
        }
    }
}

array reverse(array src) {
    if (!src) return 0;

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
        memcpy(dsts + (i * size), srcs + ((length - 1 - i) * size), size);

    return dest;
}