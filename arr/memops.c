#include <stdlib.h>
#include <string.h>
#include "arr.h"

array write(
    array dest, 
    uint64 start, 
    uint64 end, 
    any src
) {
    if (!dest || !src) return 0;
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
        if (!new_dst) return 0;

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

    return dest;
}

array insert(
    array dest, 
    uint64 start, 
    uint64 end, 
    any src
) {
    if (!dest || !src) return 0;

    if (start >= dest->length) return write(dest, start, end, src);

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
        if (!new_dst) return 0;

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

    return dest;
}

array fill(array dest, uint64 start, uint64 end, any val) {
    if (!dest || !val) return 0;

    if (start > end) {
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
        if (!new_dst) return 0;

        dest->buffer = new_dst;
        dst = new_dst;
        memset(dst + alloc_length * size, 0xFF, (new_alloc_length - alloc_length) * size);
        dest->alloc.length = new_alloc_length;
    }

    for (uint64 i = start; i < end + 1; i++)
        memcpy(
            dst + (i * size),
            val,
            size
        );

    dest->length = (dest->length > end + 1) ? dest->length : end + 1;
    return dest;
}

array erase(array dest, uint64 start, uint64 end) {
    if (!dest || start >= dest->length || end >= dest->length) return 0;
    
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
    return dest;
}

array flush(array dest, double percentage) {
    if (!dest) return 0;
    uint64 old_length = dest->alloc.length;
    uint64 size = dest->size;
    dest->alloc.length -= ((dest->alloc.length - dest->length) * (percentage / 100));

    if (dest->alloc.length != old_length) {
        raw new_buffer = (raw) realloc(dest->buffer, dest->alloc.length * size);
        if (!new_buffer) return 0;
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

    return dest;
}