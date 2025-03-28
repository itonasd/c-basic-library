#ifndef ARRAY_H
#define ARRAY_H

#include <stdlib.h>
#include <string.h>

// 40ULL
typedef struct __attribute__((packed)) array {
    unsigned char *buffer; // (0x00 -> 0x07)
    size_t length; // (0x08 -> 0x0F)
    size_t size; // (0x10 -> 0x17)

    struct config {
        size_t pre_allocation_factor; // memory pre-allocation on expand (default: 0) (0x18 -> 0x1F)
        unsigned char __0x20; // reserved field (0x20)
        unsigned char default_cell_value; // uninitialized cell value (default: 0xFF) (0x21)
        unsigned char write_preference_mode; // 0: overwrite | 1: insertion (default: 0) (0x22)
        unsigned char erase_preference_mode; // 0: leave as default value | 1: realign + shrink (default: 1) (0x23)
        unsigned char search_return_as; // 0: return as index | 1: return as boolean (default: 0) (0x24)
        unsigned char __0x25; // reserved field (0x25)
        unsigned char __0x26; // reserved field (0x26)
        unsigned char __0x27; // reserved field (0x27)
    } config;

} array; 

void arr_config(array* _dest, size_t size) {
    _dest->buffer = 0x0;
    _dest->length = 0;
    _dest->size = size;

    // configurations
    _dest->config.default_cell_value = 0xFF;
    _dest->config.write_preference_mode = 0;
    _dest->config.erase_preference_mode = 1;
    _dest->config.search_return_as = 0;
    _dest->config.pre_allocation_factor = 0;
    _dest->config.__0x25 = 0;
    _dest->config.__0x26 = 0;
    _dest->config.__0x27 = 0;
}

array *arr_init(size_t size) {
    array *_dest = (array *) malloc(sizeof(array));
    arr_config(_dest, size);

    return _dest;
}

size_t find_preallocated_space(array *dest) {
    size_t preallocated = 0;

    if (dest->buffer) {
        unsigned char compare[dest->size];
        memset(compare, dest->config.default_cell_value, dest->size);
        unsigned char *arr_itr = dest->buffer + (dest->length - 1) * dest->size;

        for (;arr_itr >= dest->buffer; arr_itr -= dest->size) 
        if (memcmp(arr_itr, compare, dest->size) == 0) preallocated++; else break;
    }

    return preallocated;
}

void write_s (
    array *_dest, 
    ssize_t *_st, 
    ssize_t *_en, 
    size_t _realloc, 
    unsigned char _0xfill, 
    unsigned char _insert,
    size_t _count,
    void **_src
) {
    if (!_dest || !_src || !_st || !_en) return;
    
    unsigned char swaps[_count];
    size_t highest_end = 0;
    
    for (size_t i = 0; i < _count; i++) {
        if (_st[i] == -1) _st[i] = _dest->length - 1;
        if (_en[i] == -1) _en[i] = _dest->length - 1;
        if (_st[i] < 0 || _en[i] < 0) return;
        
        swaps[i] = 0;
        if (_st[i] > _en[i]) {
            size_t temp = _st[i];
            _st[i] = _en[i];
            _en[i] = temp;
            swaps[i] = 1;
        }
        
        highest_end = (_en[i] > highest_end) ? _en[i] : highest_end;
    }
    
    size_t size = _dest->size;
    if (_dest->buffer == 0x0 && _insert) _insert = 0;
    
    if ((_dest->buffer == 0x0 || highest_end >= _dest->length) && !_insert) {
        size_t new_length = (highest_end + 1) + _realloc;
        size_t current_size = _dest->length * size;
        size_t new_size = new_length * size;

        unsigned char *new_buffer = (unsigned char *) realloc(_dest->buffer, new_size);
        if (!new_buffer) return;

        _dest->buffer = new_buffer;
        memset(_dest->buffer + current_size, _0xfill, new_size - current_size);
        _dest->length = new_length;
    }

    if (_insert) {
        size_t required_space[_count];
        size_t total_req_space = 0;
        size_t prealloated_space = find_preallocated_space(_dest);
        unsigned char compare[size];
        memset(compare, _0xfill, size);

        for (size_t i = 0; i < _count; i++) {
            required_space[i] = 0;
            unsigned char *buffer_ptr = _dest->buffer + (_st[i] * size);
            for (size_t j = _st[i]; j <= _en[i]; j++, buffer_ptr += size) {
                if (memcmp(buffer_ptr, compare, size) != 0) {
                    required_space[i]++;
                    total_req_space++;
                }
            }
        }
        
        size_t old_length = 0;
        size_t new_length = 0;
        size_t old_size = 0;
        size_t new_size = 0;
        
        if ((ssize_t) total_req_space - (ssize_t) prealloated_space > 0) {
            old_length = _dest->length;
            new_length = _dest->length + total_req_space + _realloc - prealloated_space;
            old_size = old_length * size;
            new_size = new_length * size;
            
            unsigned char* new_array = (unsigned char *) realloc(_dest->buffer, new_size);
            if (!new_array) return;
            
            _dest->buffer = new_array;
            _dest->length = new_length;
            
            memset(_dest->buffer + old_size, _0xfill, new_size - old_size);
        }
        
        if (total_req_space) { 
            size_t iterator_high;
            size_t iterator_low;

            if ((ssize_t) total_req_space - (ssize_t) prealloated_space > 0) { // in case allocated more space
                iterator_high = (old_length - 1) + required_space[0];
                iterator_low = old_length - 1;
            } else { // in case did not allocate more space
                iterator_high = ((_dest->length - 1) - prealloated_space) + required_space[0];
                iterator_low = (_dest->length - 1) - prealloated_space;
            }

            size_t origin_iterator_high = iterator_high;
            
            for (size_t i = 0; i < _count;) {
                if (iterator_low <= _en[i]) if (memcmp(_dest->buffer + (iterator_low * size), compare, size) == 0) {
                    if (iterator_low == _st[i]) goto here;
                    iterator_low--;
                    continue;
                }
                
                memmove(_dest->buffer + (iterator_high * size), _dest->buffer + (iterator_low * size), size);
                iterator_high--;
                
                if (iterator_low == _st[i]) {
                    here:
                    i++;

                    iterator_high = origin_iterator_high + required_space[i];
                    iterator_low = origin_iterator_high;
                    origin_iterator_high = iterator_high;
                } else iterator_low--;
            }
        }
    }
    
    for (size_t i = 0; i < _count; i++) {
        if (swaps[i]) {
            for (size_t j = 0; j < (_en[i] - _st[i] + 1); j++) {
                memcpy(
                    _dest->buffer + (_st[i] + j) * size,
                    (unsigned char *) _src[i] + (_en[i] - _st[i] - j) * size,
                    size
                );
            }
        } else {
            memcpy(
                _dest->buffer + _st[i] * size, 
                (unsigned char *)_src[i],
                (_en[i] - _st[i] + 1) * size
            );
        }
    }
}

void align_s(array *_dest, unsigned char _0xfill) {
    if (!_dest) return;

    size_t size = _dest->size;
    unsigned char compare[size];
    memset(compare, _0xfill, size);

    size_t indx = 0;
    for (size_t i = 0; i < _dest->length; i++) {
        if (memcmp(_dest->buffer + (i * size), compare, size) != 0) {
            if (i != indx) {
                memmove(_dest->buffer + (indx * size), _dest->buffer + (i * size), size);
                memset(_dest->buffer + (i * size), _0xfill, size);
            }
            indx++;
        }
    }
}

void erase_s(
    array *_dest, 
    ssize_t *_st, 
    ssize_t *_en, 
    unsigned char _shrink,
    unsigned char _0xfill,
    size_t _count
) {
    if (!_dest || !_st || !_en) return;
    size_t size = _dest->size;
    size_t new_length = _dest->length;

    for (size_t i = 0; i < _count; i++) {
        if (_st[i] == -1) _st[i] = _dest->length - 1;
        if (_en[i] == -1) _en[i] = _dest->length - 1;
        if (_st[i] < 0 || _en[i] < 0) return;
        if (_st[i] > _dest->length - 1 || _en[i] > _dest->length - 1) return;

        if (_st[i] > _en[i]) {
            size_t temp = _st[i];
            _st[i] = _en[i];
            _en[i] = temp;
        }
        new_length -= 1 + (_en[i] - _st[i]);
        memset(_dest->buffer + (_st[i] * size), _0xfill, (_en[i] - _st[i] + 1) * size);
    }

    align_s(_dest, _0xfill);

    if (_shrink) {
        unsigned char *new_array = (unsigned char *) realloc(_dest->buffer, new_length * size);
        if (!new_array) return;

        _dest->buffer = new_array;
        _dest->length = new_length;
    }
}

array *retrieve_s(
    array *_dest, 
    ssize_t *_st, 
    ssize_t *_en, 
    size_t _count
) {
    if (!_dest || !_st || !_en) return 0x0;
    size_t copies = 0;

    for (size_t i = 0; i < _count; i++) {
        if (_st[i] == -1) _st[i] = _dest->length - 1;
        if (_en[i] == -1) _en[i] = _dest->length - 1;
        if (_st[i] < 0 || _en[i] < 0) return 0x0;
        if (_st[i] > _dest->length - 1 || _en[i] > _dest->length - 1) return 0x0;
        if (_st)

        if (_st[i] > _en[i]) {
            size_t temp = _st[i];
            _st[i] = _en[i];
            _en[i] = temp;
        }

        copies += (_en[i] - _st[i]) + 1;
    }

    array *new_array = arr_init(_dest->size);
    if (!new_array) return 0x0;

    size_t size = _dest->size;
    new_array->buffer = (unsigned char *) malloc(copies * size);
    if (!new_array->buffer) return 0x0;

    new_array->size = size;
    new_array->length = copies;

    size_t prev_location = 0;

    for (size_t i = 0; i < _count; i++) {
        memcpy(new_array->buffer + (prev_location * size), _dest->buffer + (_st[i] * size),
        (_en[i] - _st[i] + 1) * size);

        prev_location += _en[i] - _st[i] + 1;
    }

    return new_array;
}

ssize_t search_s(
    array *_dest, 
    ssize_t *_st, 
    ssize_t *_en,
    size_t _count_length,
    size_t _count,
    size_t _type,
    void *_src
) {
    if (!_dest || !_st || !_en || !_count_length || !_src) return (_type) ? 0 : -1;
    for (size_t i = 0; i < _count; i++) {
        if (_st[i] == -1) _st[i] = _dest->length - 1;
        if (_en[i] == -1) _en[i] = _dest->length - 1;
        if (_st[i] < 0 || _en[i] < 0) return (_type) ? 0 : -1;
        if (_st[i] > _dest->length - 1 || _en[i] > _dest->length - 1) return (_type) ? 0 : -1;
    }

    size_t size = _dest->size;
    size_t result = -1;

    for (size_t i = 0; i < _count; i++) {
        ssize_t step = (_st[i] <= _en[i]) ? 1 : -1;
        for (ssize_t j = _st[i]; (step == 1) ? j <= _en[i] : j >= _en[i]; j += step) {
            for (size_t k = 0; k < _count_length; k++) {
                unsigned char *dest = _dest->buffer + (j * size);
                unsigned char *src = (unsigned char *) _src + (k * size);

                if (memcmp(dest, src, size) == 0) {
                    result = j;
                    goto finish;
                }
            }
        }
    }
    finish:

    return (_type) ? (result == -1) ? 0 : 1 : result;
}

// extended method: align_s
void align(array *dest) {
    align_s(dest, dest->config.default_cell_value);
}

ssize_t *range(ssize_t st, ssize_t en) {
    ssize_t* tmp = (ssize_t *) malloc(sizeof(ssize_t) * 2);
    tmp[0] = st; tmp[1] = en;
    return tmp;
}

// extended method: write_s
void push_front(array *dest, void *src) {
    void *srcs[1] = {src};
    ssize_t st = 0;
    ssize_t en = 0;
    write_s (
        dest, &st, &en,
        dest->config.pre_allocation_factor,
        dest->config.default_cell_value,
        1, 1, srcs
    );
}

// extended method: write_s
void push_back(array *dest, void *src) {
    void *srcs[1] = {src};
    size_t preallocated = find_preallocated_space(dest);
    
    ssize_t st = dest->length - preallocated;
    ssize_t en = dest->length - preallocated;
    write_s (
        dest, &st, &en,
        dest->config.pre_allocation_factor,
        dest->config.default_cell_value,
        0, 1, srcs
    );
}

// extended method: erase_s
void pop_back(array *dest) {
    size_t preallocated = find_preallocated_space(dest);
    
    ssize_t st = dest->length - preallocated - 1;
    ssize_t en = dest->length - preallocated - 1;
    erase_s (
        dest, &st, &en, 
        0, 
        dest->config.default_cell_value, 
        1
    );
}

// extended method: erase_s
void pop_front(array *dest) {
    ssize_t st = 0;
    ssize_t en = 0;
    erase_s (
        dest, &st, &en, 
        0, 
        dest->config.default_cell_value, 
        1
    );
}

// extended method: write_s
void write(array *dest, ssize_t *range, void* src) {
    void *srcs[1] = {src};
    ssize_t st = range[0];
    ssize_t en = range[1];
    write_s (
        dest, &st, &en, 
        dest->config.pre_allocation_factor, 
        dest->config.default_cell_value, 
        dest->config.write_preference_mode, 
        1, srcs
    );
    free(range);
}

// extended method: erase_s
void erase(array *dest, ssize_t *range) {
    ssize_t st = range[0];
    ssize_t en = range[1];
    erase_s (
        dest, &st, &en, 
        dest->config.erase_preference_mode, 
        dest->config.default_cell_value, 
        1
    );
    free(range);
}

// extended method: retrieve_s
array *retrieve(array *dest, ssize_t *range) {
    ssize_t st = range[0];
    ssize_t en = range[1];
    return retrieve_s(dest, &st, &en, 1);
    free(range);
}

// extended method: search_s
ssize_t search(array *dest, ssize_t *range, ssize_t src) {
    ssize_t srcs[1] = {src};
    ssize_t st = range[0];
    ssize_t en = range[1];
    return search_s(dest, &st, &en, 1, 1, dest->config.search_return_as, (void *) srcs);
    free(range);
}

/*
void array_log(array *buffer) {
    unsigned char *byte = (unsigned char *)buffer->buffer;
    int *_int = (int *)buffer->buffer;
    
    printf("\n    ARRAY LOG\n\n");

    for (int i = 0; i < buffer->length; i++) {
        printf("    cell %2d: [ ", i);
        for (int j = 0; j < buffer->size; j++) {
            printf("0x%02x ", byte[i * buffer->size + j]);
        }
        printf("]  %d\n", _int[i]);
    }
    printf("\n");
    printf("    size: %ld, length: %ld, struct_segment: ", buffer->size, buffer->length);
    if ((uintptr_t) buffer >= HEAP_START && (uintptr_t) buffer <= HEAP_END) printf("HEAP\n"); else printf("STACK\n"); // ONLY AVALIABLE TO LINUX
    printf("    buffer start address : %p : %d\n", buffer->buffer, 0);
    printf("    buffer end address   : %p : %ld\n\n", buffer->buffer + (buffer->size * buffer->length) - 1, (buffer->size * buffer->length) - 1);
    printf("--------------------------------------------------\n");
}

void region_log(node *buffer) {
    for (int i = 0; i < buffer->size; i++) {
        if (i == 0) printf("   buffer pointer region: |");
        if (i == 8) printf("         length region: |");
        if (i == 16) printf("           size region: |");
        if (i == 24) printf("    realloc fac region: |");
        if (i == 32) printf("  configuration region: |");

        printf(" 0x%02x %2d |", buffer->data[i], i);

        if (i == 7) printf("\n");
        if (i == 15) printf("\n");
        if (i == 23) printf("\n");
        if (i == 31) printf("\n");
        if (i == 39) printf("\n\n");
    }
}
*/

#endif