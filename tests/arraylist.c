/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "arraylist.h"
#include "../src/arraylist.h"

#define mock_memmgr _choco_arraylist_mock_memmgr
#define mock_alloc _choco_arraylist_mock_alloc
#define mock_arraylist _choco_arraylist_mock
#define mock_dealloc _choco_arraylist_mock_dealloc

void* mock_alloc(void* obj, size_t size, _choco_arraylist_result* out)
{
    struct mock_memmgr* mock = obj;
    struct mock_alloc* alloc = &mock->alloc_values[mock->alloc_calls++];
    alloc->recieved_size = size;
    *out = alloc->out_value;
    return alloc->return_pointer;
}

void mock_dealloc(void* obj, void* ptr, _choco_arraylist_result* out)
{
    struct mock_memmgr* mock = obj;
    struct mock_dealloc* dealloc = &mock->dealloc_values[mock->dealloc_calls++];
    dealloc->recieved_pointer = ptr;
    *out = dealloc->out_value;
}

void _choco_arraylist_test(void)
{
    _choco_arraylist_test_create();
    _choco_arraylist_test_clone();
    _choco_arraylist_test_resize();
}

// static arraylist create(memmgr memory, size_t units, size_t size, result* out);
// static arraylist clone(memmgr memory, arraylist other, size_t index, size_t size, result* out);
// static arraylist push(arraylist self, void* ref, size_t units, result* out);
// static arraylist pull(arraylist self, arraylist other, size_t index, size_t size, result* out);
// static arraylist reduce(arraylist self, result* out);
// static size_t size_of(arraylist self, result* out);
// static size_t length(arraylist self, result* out);
// static size_t units(arraylist self, result* out);
// static size_t size(arraylist self, result* out);
// static void* remove(arraylist self, void* ref, size_t units, result* out);
// static void* first(arraylist self, result* out);
// static void* last(arraylist self, result* out);
// static void* at(arraylist self, size_t index, result* out);
// static void destroy(arraylist self, result* out);
// static void fill(arraylist self, void* ref, size_t units, size_t index, size_t size, result* out);
// static void copy(arraylist self, arraylist other, size_t index, size_t size, result* out);
// static void clear(arraylist self, result* out);
// static void swap(arraylist self, size_t index, size_t other, result* out);
// static void flip(arraylist self, result* out);
// static void sort(arraylist self, size_t units, size_t offset, result* out);
// static int is_empty(arraylist self, result* out);
// static int equals(arraylist self, arraylist other, result* out);
// static int pop(arraylist self, size_t size, result* out);
