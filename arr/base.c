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

void item_free(items src) {
    if (!src) return;

    free(src->value);
    free(src);
}

void res_free(result src) {
    if (!src) return;

    for (uint64 i = 0; i < src->length; i++)
        free(src->item[i].value);
    free(src->item);
    free(src);
}