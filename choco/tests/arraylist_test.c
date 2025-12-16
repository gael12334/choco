/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "arraylist_test.h"
#include "../src/arraylist.h"

// Packing struct to avoid failing tests because of automatic padding. When created with
// `_choco_arraylist_create_x`, the arraylist is dynamically allocated with a packed alignment.
//
// Source : https://www.ibm.com/docs/fr/xl-c-and-cpp-aix/16.1.0?topic=descriptions-pragma-pack
//

#pragma push
#pragma pack(1)
typedef struct _mock _mock;
struct _mock {
    _choco_arraylist_header header;
    int data[10];
};
#pragma pop

static _mock init_new_mock(size_t allocated, size_t used, _choco_arraylist_allocator allocator) {
    _mock mock = {
        .header = {
            .allocated = allocated % (sizeof(mock.data) / sizeof(mock.data[0])),
            .used = used,
            .size = sizeof(mock.data[0]),
            .allocator = allocator,
            .data = mock.data
        },
        .data = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}
    };

    return mock;
}

static struct {
    _mock mocks[10];
    int used[10];
    size_t last_alloc_req_size;
    void* last_alloc_ptr;
    void* last_dealloc_ptr;
} mock_memmgr = {
    .mocks = {0},
    .used = {0},
    .last_alloc_req_size = 0,
    .last_alloc_ptr = NULL,
    .last_dealloc_ptr = NULL
};

static void mock_dealloc(void* self, void* ptr) {
    if(ptr == NULL) {
        mock_memmgr.last_dealloc_ptr = NULL;
        return;
    }

    _mock* mock = ptr;
    size_t index = mock - &mock_memmgr.mocks[0];
    mock_memmgr.used[index] = 0;
    mock_memmgr.last_dealloc_ptr = ptr;
}

static void* mock_alloc(void* self, size_t size) {
    const static size_t max_size = sizeof(_mock);
    const static size_t memmgr_count = sizeof(mock_memmgr.mocks) / sizeof(mock_memmgr.mocks[0]);

    if(size > max_size) {
        return NULL;
    }

    for(size_t i = 0; i < memmgr_count; i++) {
        if(mock_memmgr.used[i]) {
            continue;
        }

        size_t ints_allocated = (size - sizeof(_choco_arraylist_header)) / sizeof(int);
        _choco_arraylist_allocator allocator = {.allocate = mock_alloc, .deallocate = mock_dealloc};
        void* ptr_returned = &mock_memmgr.mocks[i];
        mock_memmgr.mocks[i] = init_new_mock(ints_allocated, 0, allocator);
        mock_memmgr.used[i] = 1;
        mock_memmgr.last_alloc_ptr = ptr_returned;
        mock_memmgr.last_alloc_req_size = size;
        return ptr_returned;
    }

    mock_memmgr.last_alloc_ptr = NULL;
    mock_memmgr.last_alloc_req_size = size;
    return NULL;
}

static _choco_arraylist_allocator init_new_allocator(void)
{
    _choco_arraylist_allocator allocator = {
        .allocate = mock_alloc,
        .deallocate = mock_dealloc
    };
    return allocator;
}

static _mock mock_arrlist;
static void init_test_struct(void)
{
    mock_arrlist = (_mock) {
        .header = {
            .allocator = { .allocate = NULL, .deallocate = NULL },
            .allocated = sizeof(mock_arrlist.data) / sizeof(mock_arrlist.data[0]),
            .used = 0,
            .size = sizeof(int),
            .data = mock_arrlist.data },
        .data = { 0, 1, 2, 3, 4, 5 }
    };
}

_gt_test(_choco_arraylist_get_header, )
{
    // arrange
    init_test_struct();

    // act
    _choco_arraylist_header* header = _choco_arraylist_get_header(mock_arrlist.data);

    // assert
    _gt_test_ptr_eq(header, &mock_arrlist.header);
    _gt_test_int_eq(header->allocated, 6);
    _gt_test_int_eq(header->size, sizeof(int));
    _gt_test_ptr_eq(header->data, mock_arrlist.data);
    _gt_passed();
}

_gt_test(_choco_arraylist_length, )
{
    // arrange
    init_test_struct();
    mock_arrlist.header.used = 3;

    // act
    size_t length = _choco_arraylist_length(mock_arrlist.data);

    // assert
    _gt_test_int_eq(length, 3);
    _gt_passed();
}

_gt_test(_choco_arraylist_element_size, )
{
    // arrange
    init_test_struct();

    // act
    size_t size = _choco_arraylist_element_size(mock_arrlist.data);

    // assert
    _gt_test_int_eq(size, sizeof(int));
    _gt_passed();
}

_gt_test(_choco_arraylist_sizeof, )
{
    // arrange
    init_test_struct();

    // act
    size_t physical_size = _choco_arraylist_sizeof(mock_arrlist.data);

    // assert
    _gt_test_int_eq(physical_size, sizeof(mock_arrlist));
    _gt_passed();
}

_gt_test(_choco_arraylist_at, )
{
    // arrange
    init_test_struct();
    mock_arrlist.header.used = 4;

    int index = 2;
    int invalid_index = 5;
    int value = mock_arrlist.data[index];

    // act
    int* result_sucess = _choco_arraylist_at(mock_arrlist.data, index);
    int* result_failure = _choco_arraylist_at(mock_arrlist.data, invalid_index);

    // assert
    _gt_test_ptr_neq(result_sucess, NULL);
    _gt_test_int_eq(*result_sucess, value);
    _gt_test_ptr_eq(result_failure, NULL);
    _gt_passed();
}

_gt_test(_choco_arraylist_swap, )
{
    // arrange
    init_test_struct();
    mock_arrlist.header.used = 4;

    int index1 = 1;
    int index2 = 2;
    int value1 = mock_arrlist.data[index1];
    int value2 = mock_arrlist.data[index2];

    // act
    _choco_arraylist_swap(mock_arrlist.data, 1, 2);

    // assert
    _gt_test_int_eq(mock_arrlist.data[index1], value2);
    _gt_test_int_eq(mock_arrlist.data[index2], value1);
    _gt_passed();
}

_gt_test(_choco_arraylist_swap, invalid_index)
{
    // arrange
    init_test_struct();
    mock_arrlist.header.used = 4;

    int index1 = 1;
    int index2 = 5;
    int value1 = mock_arrlist.data[index1];

    // act
    _choco_arraylist_swap(mock_arrlist.data, index1, index2);

    // assert
    _gt_test_int_eq(mock_arrlist.data[index1], value1); // stays unchanged.
    _gt_passed();
}

_gt_test(_choco_arraylist_remove, )
{
    // arrange
    init_test_struct();
    mock_arrlist.header.used = 4;
    size_t old_used = mock_arrlist.header.used;

    // act
    _choco_arraylist_remove(mock_arrlist.data);

    // assert
    _gt_test_int_eq(mock_arrlist.header.used, old_used - 1);
    _gt_passed();
}

_gt_test(_choco_arraylist_remove, when_empty)
{
    // arrange
    init_test_struct();

    // act
    _choco_arraylist_remove(mock_arrlist.data);

    // assert
    _gt_test_int_eq(mock_arrlist.header.used, 0);
    _gt_passed();
}

_gt_test(_choco_arraylist_create, )
{
    // arrange
    _choco_arraylist_allocator allocator = init_new_allocator();
    size_t size = sizeof(int);
    size_t alloc = 1;

    // act
    _choco_arraylist arrlist = _choco_arraylist_create(allocator, size, alloc);

    // assert
    _gt_test_ptr_neq(arrlist, NULL);
    _gt_test_ptr_eq(arrlist, mock_arrlist.data);
    _gt_test_int_eq(mock_arrlist.header.allocated, alloc);
    _gt_test_int_eq(mock_alloc_last_req_res, 1);
    _gt_passed();
}

_gt_test(_choco_arraylist_create, mem_alloc_failed)
{
    // arrange
    _choco_arraylist_allocator allocator = init_new_allocator();
    size_t size = sizeof(int);
    size_t alloc = 10;
    size_t physical_size = sizeof(_choco_arraylist_header) + sizeof(int) * alloc;

    // act
    _choco_arraylist arrlist = _choco_arraylist_create(allocator, size, alloc);

    // assert
    _gt_test_ptr_eq(arrlist, NULL);
    _gt_test_int_eq(mock_alloc_last_req_sz, physical_size);
    _gt_test_int_eq(mock_alloc_last_req_res, 0);
    _gt_passed();
}

_gt_test(_choco_arraylist_resize, )
{
    // arrange
    init_test_struct();
    mock_arrlist.header.allocated = 2;
    mock_arrlist.header.allocator = init_new_allocator();
    size_t to_alloc = 5;
    size_t req_size = sizeof(mock_arrlist.header) + sizeof(int) * to_alloc;
    _choco_arraylist_header* header = &mock_arrlist.header;
    _choco_arraylist arrlist = mock_arrlist.data;

    // act
    arrlist = _choco_arraylist_resize(arrlist, to_alloc);

    // assert
    _gt_test_ptr_neq(arrlist, NULL);
    _gt_test_ptr_eq(mock_dealloc_last_ptr, header);
    _gt_test_int_eq(mock_alloc_last_req_sz, req_size);
    _gt_test_int_eq(header->allocated, to_alloc);
    _gt_test_int_eq(mock_alloc_last_req_res, 1);
    _gt_passed();
}

_gt_test(_choco_arraylist_resize, mem_alloc_failed)
{
    // arrange
    init_test_struct();
    mock_arrlist.header.allocated = 2;
    mock_arrlist.header.allocator = init_new_allocator();
    size_t to_alloc = 10;
    _choco_arraylist_header* header = &mock_arrlist.header;
    _choco_arraylist arrlist = mock_arrlist.data;

    // act
    arrlist = _choco_arraylist_resize(arrlist, to_alloc);

    // assert
    _gt_test_ptr_eq(arrlist, NULL);
    _gt_test_int_eq(mock_alloc_last_req_res, 0);
    _gt_test_ptr_eq(mock_dealloc_last_ptr, header);
    _gt_test_int_eq(header->allocated, 2);
    _gt_passed();
}

_gt_test(_choco_arraylist_resize, alloc_invalid)
{
    // arrange
    init_test_struct();
    mock_arrlist.header.allocated = 2;
    mock_arrlist.header.allocator = (_choco_arraylist_allocator) { 0 };
    mock_alloc_last_req_sz = 0;
    size_t to_alloc = 5;
    _choco_arraylist_header* header = &mock_arrlist.header;
    _choco_arraylist arrlist = mock_arrlist.data;

    // act
    arrlist = _choco_arraylist_resize(arrlist, to_alloc);

    // assert
    _gt_test_ptr_eq(arrlist, NULL);
    _gt_test_int_eq(mock_alloc_last_req_sz, 0);
    _gt_test_ptr_eq(mock_dealloc_last_ptr, header);
    _gt_test_int_eq(header->allocated, 2);
    _gt_passed();
}

_gt_test(_choco_arraylist_is_full, no)
{
    // arrange
    init_test_struct();
    mock_arrlist.header.used = 2;

    // act
    int result = _choco_arraylist_is_full(mock_arrlist.data);

    // assert
    _gt_test_int_eq(result, 0);
    _gt_passed();
}

_gt_test(_choco_arraylist_is_full, yes)
{
    // arrange
    init_test_struct();
    mock_arrlist.header.used = 10;

    // act
    int result = _choco_arraylist_is_full(mock_arrlist.data);

    // assert
    _gt_test_int_eq(result, 1);
    _gt_passed();
}

_gt_test(_choco_arraylist_add, )
{
    // arrange
    size_t alloc = 2;
    size_t used = 0;
    size_t used_after_add = used + 1;
    init_test_struct();
    _choco_arraylist_header* header = &mock_arrlist.header;
    header->allocated = alloc;
    header->used = used;
    header->allocator = init_new_allocator();
    mock_alloc_last_req_sz = 0;
    mock_dealloc_last_ptr = NULL;

    // act
    _choco_arraylist result = _choco_arraylist_add(mock_arrlist.data);

    // assert
    _gt_test_ptr_neq(result, NULL);
    _gt_test_int_eq(mock_alloc_last_req_res, 0);
    _gt_test_ptr_eq(mock_dealloc_last_ptr, NULL);
    _gt_test_int_eq(header->used, used_after_add);
    _gt_test_int_eq(header->allocated, alloc);
    _gt_passed();
}

_gt_test(_choco_arraylist_add, when_full)
{
    // arrange
    init_test_struct();
    _choco_arraylist_header* header = &mock_arrlist.header;
    _choco_arraylist arrlist = mock_arrlist.data;
    size_t alloc = 2;
    size_t used = 2;
    size_t used_after_add = used + 1;
    size_t alloc_after_add = (alloc + 1) * 2;
    size_t request_size = sizeof(int) * alloc_after_add + sizeof(*header);
    header->allocated = alloc;
    header->used = used;
    header->allocator = init_new_allocator();

    // act
    _choco_arraylist result = _choco_arraylist_add(mock_arrlist.data);

    // assert
    _gt_test_ptr_neq(result, NULL);
    _gt_test_int_eq(mock_alloc_last_req_sz, request_size);
    _gt_test_ptr_eq(mock_dealloc_last_ptr, header);
    _gt_test_int_eq(header->used, used_after_add);
    _gt_test_int_eq(header->allocated, alloc_after_add);
    _gt_passed();
}

_gt_test(_choco_arraylist_add, alloc_fail)
{
    // arrange
    init_test_struct();
    _choco_arraylist_header* header = &mock_arrlist.header;
    _choco_arraylist arrlist = mock_arrlist.data;
    size_t alloc = 6;
    size_t used = 6;
    size_t alloc_after_add = (alloc + 1) * 2;
    size_t request_size = sizeof(int) * alloc_after_add + sizeof(*header);
    header->allocated = alloc;
    header->used = used;
    header->allocator = init_new_allocator();

    // act
    _choco_arraylist result = _choco_arraylist_add(mock_arrlist.data);

    // assert
    _gt_test_ptr_eq(result, NULL);
    _gt_test_int_eq(mock_alloc_last_req_sz, request_size);
    _gt_test_ptr_eq(mock_dealloc_last_ptr, header);
    _gt_passed();
}

void _choco_arraylist_test(void)
{
    _gt_run(_choco_arraylist_get_header, );
    _gt_run(_choco_arraylist_length, );
    _gt_run(_choco_arraylist_element_size, );
    _gt_run(_choco_arraylist_sizeof, );
    _gt_run(_choco_arraylist_at, );
    _gt_run(_choco_arraylist_swap, );
    _gt_run(_choco_arraylist_swap, invalid_index);
    _gt_run(_choco_arraylist_remove, );
    _gt_run(_choco_arraylist_remove, when_empty);
    _gt_run(_choco_arraylist_create, );
    _gt_run(_choco_arraylist_create, mem_alloc_failed);
    _gt_run(_choco_arraylist_resize, );
    _gt_run(_choco_arraylist_resize, mem_alloc_failed);
    _gt_run(_choco_arraylist_resize, alloc_invalid);
    _gt_run(_choco_arraylist_is_full, no);
    _gt_run(_choco_arraylist_is_full, yes);
    _gt_run(_choco_arraylist_add, );
    _gt_run(_choco_arraylist_add, when_full);
    _gt_run(_choco_arraylist_add, alloc_fail);
}
