#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "arr.h"

void arr_config(array dest, uint64 size) {
    dest->buffer = 0x0;
    dest->length = 0;
    dest->size = size;
    dest->alloc.length = 0;
    dest->alloc.factor = 0;
}

array arr_init(uint64 size) {
    array dest = (array) malloc(sizeof(arr));
    arr_config(dest, size);
    return dest;
}

void arr_free(array src) {
    if (!src) return;

    free(src->buffer);
    free(src);
}

void arr_log(array src) {
    array buffer = src;
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