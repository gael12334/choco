/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "arraylist.h"

static unsigned _choco_arraylist_get_required_space(unsigned size, unsigned alloc)
{
    return sizeof(_choco_arraylist_header) + size * alloc;
}

static void* _choco_arraylist_heap_alloc(void* self, unsigned size)
{
    return malloc(size);
}

static void _choco_arraylist_heap_dealloc(void* self, void* ptr)
{
    free(ptr);
}

_choco_arraylist_allocator _choco_arraylist_heap_allocator(void)
{
    _choco_arraylist_allocator allocator = {
        .allocate = _choco_arraylist_heap_alloc,
        .dealloate = _choco_arraylist_heap_dealloc
    };
    return allocator;
}

_choco_arraylist _choco_arraylist_create_x(_choco_arraylist_allocator allocator, unsigned size, unsigned allocated)
{
    _choco_arraylist_header header = {
        .allocated = allocated,
        .allocator = allocator,
        .data = NULL,
        .size = size,
        .used = 0
    };
    unsigned required_space = _choco_arraylist_get_required_space(size, allocated);
    int is_allocator_valid = (allocator.allocate != NULL && allocator.dealloate != NULL);
    void* memory = (is_allocator_valid) ? allocator.allocate(&allocator, required_space) : NULL;
    int is_memory_valid = (memory != NULL);
    header.data = memory + sizeof(header);
    memcpy(memory, &header, sizeof(header) * is_memory_valid);
    memset(header.data, 0, size * allocated * is_memory_valid);
    return (is_memory_valid) ? header.data : NULL;
}

_choco_arraylist_header* _choco_arraylist_get_header(_choco_arraylist arrlist)
{
    return ((_choco_arraylist_header*)arrlist) - 1;
}

unsigned _choco_arraylist_sizeof(_choco_arraylist arrlist)
{
    _choco_arraylist_header* header = _choco_arraylist_get_header(arrlist);
    return sizeof(*header) + header->size * header->allocated;
}

unsigned _choco_arraylist_length(_choco_arraylist arrlist)
{
    _choco_arraylist_header* header = _choco_arraylist_get_header(arrlist);
    return header->used;
}

unsigned _choco_arraylist_element_size(_choco_arraylist arrlist)
{
    _choco_arraylist_header* header = _choco_arraylist_get_header(arrlist);
    return header->size;
}

void _choco_arraylist_destroy(_choco_arraylist arrlist)
{
    _choco_arraylist_header* header = _choco_arraylist_get_header(arrlist);
    unsigned size = _choco_arraylist_sizeof(arrlist);
    memset(header, 0, size);
    free(header);
}

void* _choco_arraylist_at(_choco_arraylist arrlist, unsigned index)
{
    _choco_arraylist_header* header = _choco_arraylist_get_header(arrlist);
    void* element = arrlist + header->size * index;
    return (index >= header->used) ? NULL : element;
}

_choco_arraylist _choco_arraylist_resize(_choco_arraylist arrlist, unsigned desired_alloc)
{
    _choco_arraylist_header header = *_choco_arraylist_get_header(arrlist);
    _choco_arraylist_allocator allocator = header.allocator;
    int is_allocator_valid = (allocator.allocate != NULL && allocator.dealloate != NULL);
    unsigned required_space = _choco_arraylist_get_required_space(header.size, desired_alloc);
    unsigned used_space = _choco_arraylist_sizeof(arrlist);
    void* new_header = (is_allocator_valid) ? allocator.allocate(&allocator, required_space) : NULL;
    int is_memory_valid = (new_header != NULL);
    void* new_arrlist = new_header + sizeof(header);
    memcpy(new_arrlist, arrlist, header.size * desired_alloc * is_memory_valid);
    header.data = new_arrlist;
    header.allocated = desired_alloc;
    memcpy(new_header, &header, sizeof(header) * is_memory_valid);
    void* old_memory = _choco_arraylist_get_header(arrlist);
    if (is_allocator_valid) {
        allocator.dealloate(&allocator, old_memory);
    }
    return (is_memory_valid) ? new_arrlist : NULL;
}

_choco_arraylist _choco_arraylist_add(_choco_arraylist arrlist)
{
    _choco_arraylist_header* header = _choco_arraylist_get_header(arrlist);
    _choco_arraylist_header new_header = *header;
    unsigned desired_alloc = (header->allocated + 1) * 2;
    int is_full = _choco_arraylist_is_full(arrlist);
    arrlist = (is_full) ? _choco_arraylist_resize(arrlist, desired_alloc) : arrlist;
    int is_arrlist_valid = (arrlist != NULL);
    new_header.data = arrlist;
    new_header.allocated = (is_full) ? desired_alloc : new_header.allocated;
    new_header.used++;
    header = (is_arrlist_valid) ? _choco_arraylist_get_header(arrlist) : NULL;
    memcpy(header, &new_header, sizeof(*header) * is_arrlist_valid);
    return arrlist;
}

void _choco_arraylist_remove(_choco_arraylist arrlist)
{
    _choco_arraylist_header* header = _choco_arraylist_get_header(arrlist);
    header->used -= (header->used > 0);
}

void _choco_arraylist_swap(_choco_arraylist arrlist, unsigned a, unsigned b)
{
    _choco_arraylist_header* header = _choco_arraylist_get_header(arrlist);
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
    _choco_arraylist_header* header = _choco_arraylist_get_header(arrlist);
    int is_full = header->used >= header->allocated;
    return is_full;
}
