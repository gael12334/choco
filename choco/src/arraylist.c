/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "arraylist.h"
#include <asm-generic/errno.h>

typedef _choco_arraylist_header _header;
typedef _choco_arraylist_result _result;
typedef _choco_arraylist_allocator _allocator;

#define _get_element(arrlist, size, index) \
    (arrlist + (size * index))

#define _physical_size(size, alloc) \
    (sizeof(_header) + size * alloc)

#define _is_allocator_valid(allocator) \
    (allocator.allocate != NULL && allocator.deallocate != NULL)

#define _get_header(arrlist) \
    (((_header*)arrlist) - 1)

static void* _heap_alloc(void* self, size_t size)
{
    return malloc(size);
}

static void _heap_dealloc(void* self, void* ptr)
{
    free(ptr);
}

_allocator _choco_arraylist_heap_allocator(void)
{
    _allocator allocator = {
        .allocate = _heap_alloc,
        .deallocate = _heap_dealloc
    };
    return allocator;
}

_choco_arraylist _choco_arraylist_create(_allocator allocator, size_t size, size_t desired)
{
    if (!_is_allocator_valid(allocator)) {
        return NULL;
    }

    size_t required_space = _physical_size(size, desired);
    _header* header = allocator.allocate(&allocator, required_space);
    if (header == NULL) {
        return NULL;
    }

    *header = (_header) {
        .allocated = desired,
        .allocator = allocator,
        .data = header + 1,
        .size = size,
        .used = 0
    };

    return header->data;
}

_header* _choco_arraylist_get_header(_choco_arraylist arrlist)
{
    if (arrlist == NULL) {
        return NULL;
    }

    return ((_header*)arrlist) - 1;
}

size_t _choco_arraylist_sizeof(_choco_arraylist arrlist)
{
    if (arrlist == NULL) {
        return 0;
    }

    _header* header = _choco_arraylist_get_header(arrlist);
    return _physical_size(header->size, header->allocated);
}

size_t _choco_arraylist_length(_choco_arraylist arrlist)
{
    if (arrlist == NULL) {
        return 0;
    }

    _header* header = _get_header(arrlist);
    return header->used;
}

size_t _choco_arraylist_element_size(_choco_arraylist arrlist)
{
    if (arrlist == NULL) {
        return 0;
    }

    _header* header = _choco_arraylist_get_header(arrlist);
    return header->size;
}

_result _choco_arraylist_destroy(_choco_arraylist arrlist)
{
    if (arrlist == NULL) {
        return _CHOCO_ARRAYLIST_RESULT_ERROR;
    }

    _header* header = _choco_arraylist_get_header(arrlist);

    if (!_is_allocator_valid(header->allocator)) {
        return _CHOCO_ARRAYLIST_RESULT_ERROR;
    }

    size_t size = _choco_arraylist_sizeof(arrlist);
    memset(header, 0, size);
    header->allocator.deallocate(&header->allocator, header);
    return _CHOCO_ARRAYLIST_RESULT_OK;
}

void* _choco_arraylist_at(_choco_arraylist arrlist, size_t index)
{
    if (arrlist == NULL) {
        return NULL;
    }

    _header* header = _choco_arraylist_get_header(arrlist);

    if (index >= header->used) {
        return NULL;
    }

    return _get_element(arrlist, header->size, index);
}

_choco_arraylist _choco_arraylist_resize(_choco_arraylist arrlist, size_t desired)
{
    if (arrlist == NULL) {
        return NULL;
    }

    _header* header = _choco_arraylist_get_header(arrlist);
    _allocator allocator = header->allocator;
    int is_allocator_valid = _is_allocator_valid(allocator);
    if (!is_allocator_valid) {
        return arrlist;
    }

    size_t desired_size = _physical_size(header->size, desired);
    _header* new_header = allocator.allocate(&allocator, desired_size);
    if (new_header == NULL) {
        return arrlist;
    }

    *new_header = (_header) {
        .allocated = desired,
        .allocator = allocator,
        .size = header->size,
        .used = header->used,
        .data = new_header + 1,
    };



    allocator.deallocate(&allocator, header);
    return new_header->data;
}

_choco_arraylist _choco_arraylist_add(_choco_arraylist arrlist)
{
    if (arrlist == NULL) {
        return NULL;
    }

    _header* header = _choco_arraylist_get_header(arrlist);
    _result is_full = _choco_arraylist_is_full(arrlist);

    if (is_full == _CHOCO_ARRAYLIST_RESULT_YES) {
        arrlist = _choco_arraylist_resize(arrlist, (header->allocated + 1) * 2);
        header = _choco_arraylist_get_header(arrlist);
    }

    void* element = _get_element(arrlist, header->size, header->used++);
    memset(element, 0, header->size);
    return arrlist;
}

_result _choco_arraylist_remove(_choco_arraylist arrlist)
{
    if (arrlist == NULL) {
        return _CHOCO_ARRAYLIST_RESULT_ERROR;
    }

    _header* header = _choco_arraylist_get_header(arrlist);

    if (header->used == 0) {
        return _CHOCO_ARRAYLIST_RESULT_ERROR;
    }

    header->used--;
    return _CHOCO_ARRAYLIST_RESULT_OK;
}

_result _choco_arraylist_swap(_choco_arraylist arrlist, size_t a, size_t b)
{
    if (arrlist == NULL) {
        return _CHOCO_ARRAYLIST_RESULT_ERROR;
    }

    _header* header = _choco_arraylist_get_header(arrlist);

    if (a >= header->used || b >= header->used) {
        return _CHOCO_ARRAYLIST_RESULT_ERROR;
    }

    char temp[header->size];
    void* element_a = _get_element(arrlist, header->size, a);
    void* element_b = _get_element(arrlist, header->size, b);
    memcpy(temp, element_a, header->size);
    memcpy(element_a, element_b, header->size);
    memcpy(element_b, temp, header->size);
    return _CHOCO_ARRAYLIST_RESULT_OK;
}

_result _choco_arraylist_is_full(_choco_arraylist arrlist)
{
    if (arrlist == NULL) {
        return _CHOCO_ARRAYLIST_RESULT_ERROR;
    }

    _header* header = _choco_arraylist_get_header(arrlist);
    return (header->used >= header->allocated) ? _CHOCO_ARRAYLIST_RESULT_YES : _CHOCO_ARRAYLIST_RESULT_NO;
}
