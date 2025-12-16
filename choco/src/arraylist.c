/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "arraylist.h"

typedef _choco_arraylist_header _header;
typedef _choco_arraylist_allocator _allocator;

static size_t _choco_arraylist_physical_size(size_t size, size_t alloc)
{
    return sizeof(_header) + size * alloc;
}

static int _choco_arraylist_is_allocator_valid(_allocator allocator)
{
    return allocator.allocate != NULL && allocator.deallocate != NULL;
}

static void* _choco_arraylist_heap_alloc(void* self, size_t size)
{
    return malloc(size);
}

static void _choco_arraylist_heap_dealloc(void* self, void* ptr)
{
    free(ptr);
}

_allocator _choco_arraylist_heap_allocator(void)
{
    _allocator allocator = {
        .allocate = _choco_arraylist_heap_alloc,
        .deallocate = _choco_arraylist_heap_dealloc
    };
    return allocator;
}

_choco_arraylist _choco_arraylist_create(_allocator allocator, size_t size, size_t allocated)
{
    if (allocator.allocate == NULL || allocator.deallocate == NULL) {
        return NULL;
    }

    size_t required_space = _choco_arraylist_physical_size(size, allocated);
    _header* header = allocator.allocate(&allocator, required_space);
    if(header == NULL) {
        return NULL;
    }

    *header = (_header) {
        .allocated = allocated,
        .allocator = allocator,
        .data = header + 1,
        .size = size,
        .used = 0
    };

    return header->data;
}

_header* _choco_arraylist_get_header(_choco_arraylist arrlist)
{
    return ((_header*)arrlist) - 1;
}

size_t _choco_arraylist_sizeof(_choco_arraylist arrlist)
{
    _header* header = _choco_arraylist_get_header(arrlist);
    return _choco_arraylist_physical_size(header->size, header->allocated);
}

size_t _choco_arraylist_length(_choco_arraylist arrlist)
{
    _header* header = _choco_arraylist_get_header(arrlist);
    return header->used;
}

size_t _choco_arraylist_element_size(_choco_arraylist arrlist)
{
    _header* header = _choco_arraylist_get_header(arrlist);
    return header->size;
}

void _choco_arraylist_destroy(_choco_arraylist arrlist)
{
    _header* header = _choco_arraylist_get_header(arrlist);
    size_t size = _choco_arraylist_sizeof(arrlist);
    memset(header, 0, size);
    free(header);
}

void* _choco_arraylist_at(_choco_arraylist arrlist, size_t index)
{
    _header* header = _choco_arraylist_get_header(arrlist);
    return (index < header->used) ? arrlist + header->size * index : NULL;
}

_choco_arraylist _choco_arraylist_resize(_choco_arraylist arrlist, size_t desired)
{
    if (arrlist == NULL) {
        return arrlist;
    }

    _header* header = _choco_arraylist_get_header(arrlist);
    _allocator allocator = header->allocator;
    int is_allocator_valid = _choco_arraylist_is_allocator_valid(allocator);
    if (!is_allocator_valid) {
        return arrlist;
    }

    size_t desired_size = _choco_arraylist_physical_size(header->size, header->allocated);
    _header* new_header = allocator.allocate(&allocator, desired_size);
    if (new_header == NULL) {
        return arrlist;
    }

    new_header->allocated = desired;
    new_header->data = new_header + 1;
    new_header->allocator = allocator;
    new_header->size = header->size;
    new_header->used = header->used;

    size_t smallest = (desired > header->allocated) ? header->allocated : desired;
    memcpy(new_header->data, header->data, smallest * header->size);
    allocator.deallocate(&allocator, header);
    return new_header->data;
}

_choco_arraylist _choco_arraylist_add(_choco_arraylist arrlist)
{
    if (arrlist == NULL) {
        return arrlist;
    }

    _header* header = _choco_arraylist_get_header(arrlist);
    int is_full = _choco_arraylist_is_full(arrlist);
    if (is_full) {
        arrlist = _choco_arraylist_resize(arrlist, (header->allocated + 1) * 2);
        header = _choco_arraylist_get_header(arrlist);
    }

    header->used++;
    void* element = _choco_arraylist_at(arrlist, header->used - 1);
    memset(element, 0, header->size);
    return arrlist;
}

void _choco_arraylist_remove(_choco_arraylist arrlist)
{
    _header* header = _choco_arraylist_get_header(arrlist);
    header->used -= (header->used > 0);
}

void _choco_arraylist_swap(_choco_arraylist arrlist, size_t a, size_t b)
{
    _header* header = _choco_arraylist_get_header(arrlist);

    if (a >= header->used || b >= header->used) {
        return;
    }

    char temp[header->size];
    void* element_a = _choco_arraylist_at(arrlist, a);
    void* element_b = _choco_arraylist_at(arrlist, b);
    memcpy(temp, element_a, header->size);
    memcpy(element_a, element_b, header->size);
    memcpy(element_b, temp, header->size);
}

int _choco_arraylist_is_full(_choco_arraylist arrlist)
{
    _header* header = _choco_arraylist_get_header(arrlist);
    int is_full = header->used >= header->allocated;
    return is_full;
}
