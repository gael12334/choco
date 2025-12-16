/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void* _choco_arraylist;

typedef enum _choco_arraylist_result {
    _CHOCO_ARRAYLIST_RESULT_OK,
    _CHOCO_ARRAYLIST_RESULT_ERROR,
    _CHOCO_ARRAYLIST_RESULT_YES,
    _CHOCO_ARRAYLIST_RESULT_NO,
} _choco_arraylist_result;

typedef struct _choco_arraylist_allocator {
    void*(*allocate)(void* self, size_t size);
    void(*deallocate)(void* self, void* ptr);
} _choco_arraylist_allocator;

typedef struct _choco_arraylist_header {
    _choco_arraylist data;
    _choco_arraylist_allocator allocator;
    size_t allocated;
    size_t used;
    size_t size;
} _choco_arraylist_header;

_choco_arraylist_allocator _choco_arraylist_heap_allocator(void);
_choco_arraylist_header* _choco_arraylist_get_header(_choco_arraylist arrlist);
_choco_arraylist_result _choco_arraylist_destroy(_choco_arraylist arrlist);
_choco_arraylist_result _choco_arraylist_remove(_choco_arraylist arrlist);
_choco_arraylist_result _choco_arraylist_swap(_choco_arraylist arrlist, size_t a, size_t b);
_choco_arraylist_result _choco_arraylist_is_full(_choco_arraylist arrlist);
_choco_arraylist _choco_arraylist_create(_choco_arraylist_allocator allocator, size_t size, size_t allocated);
_choco_arraylist _choco_arraylist_resize(_choco_arraylist arrlist, size_t desired);
_choco_arraylist _choco_arraylist_add(_choco_arraylist arrlist);
size_t _choco_arraylist_element_size(_choco_arraylist arrlist);
size_t _choco_arraylist_sizeof(_choco_arraylist arrlist);
size_t _choco_arraylist_length(_choco_arraylist arrlist);
void* _choco_arraylist_at(_choco_arraylist arrlist, size_t index);
