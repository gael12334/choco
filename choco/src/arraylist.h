/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef void* _choco_arraylist;

typedef struct _choco_arraylist_allocator _choco_arraylist_allocator;
struct _choco_arraylist_allocator {
    void*(*allocate)(void* self, unsigned size);
    void(*dealloate)(void* self, void* ptr);
};

typedef struct _choco_arraylist_header _choco_arraylist_header;
struct _choco_arraylist_header {
    _choco_arraylist data;
    _choco_arraylist_allocator allocator;
    unsigned allocated;
    unsigned used;
    unsigned size;
};

_choco_arraylist_allocator _choco_arraylist_heap_allocator(void);
_choco_arraylist _choco_arraylist_create_x(_choco_arraylist_allocator allocator, unsigned size, unsigned allocated);
_choco_arraylist_header* _choco_arraylist_get_header(_choco_arraylist arrlist);
unsigned _choco_arraylist_sizeof(_choco_arraylist arrlist);
unsigned _choco_arraylist_length(_choco_arraylist arrlist);
unsigned _choco_arraylist_element_size(_choco_arraylist arrlist);
void _choco_arraylist_destroy(_choco_arraylist arrlist);
void* _choco_arraylist_at(_choco_arraylist arrlist, unsigned index);
_choco_arraylist _choco_arraylist_resize(_choco_arraylist arrlist, unsigned desired_alloc);
_choco_arraylist _choco_arraylist_add(_choco_arraylist arrlist);
void _choco_arraylist_remove(_choco_arraylist arrlist);
void _choco_arraylist_swap(_choco_arraylist arrlist, unsigned a, unsigned b);
int _choco_arraylist_is_full(_choco_arraylist arrlist);
