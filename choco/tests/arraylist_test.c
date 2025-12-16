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
#define _MOCK_DATA_LENGTH (10)
typedef struct _mock _mock;
struct _mock {
    _choco_arraylist_header header;
    int data[_MOCK_DATA_LENGTH];
};
#pragma pop

static struct _memmgr {
    _mock mocks[10];
    int used[10];
    size_t a_last_req_size;
    void* a_last_ptr;
    void* d_last_ptr;
} mock_memmgr = {
    .mocks = { 0 },
    .used = { 0 },
    .a_last_req_size = 0,
    .a_last_ptr = NULL,
    .d_last_ptr = NULL
};

static void init_new_mock(_mock* mock, size_t allocated, size_t used, _choco_arraylist_allocator allocator)
{
    int data[_MOCK_DATA_LENGTH];
    for (size_t i = 0; i < _MOCK_DATA_LENGTH; i++) {
        data[i] = i;
    }

    memcpy(mock->data, data, sizeof(data));
    mock->header.data = &mock->data[0];
    mock->header.allocated = allocated;
    mock->header.size = sizeof(int);
    mock->header.used = used;
}

static void mock_dealloc(void* self, void* ptr)
{
    if (ptr == NULL) {
        mock_memmgr.d_last_ptr = NULL;
        return;
    }

    _mock* mock = ptr;
    size_t index = mock - &mock_memmgr.mocks[0];
    mock_memmgr.used[index] = 0;
    mock_memmgr.d_last_ptr = ptr;
}

static void* mock_alloc(void* self, size_t size)
{
    const static size_t max_size = sizeof(_mock);
    const static size_t memmgr_count = sizeof(mock_memmgr.mocks) / sizeof(mock_memmgr.mocks[0]);
    void* ptr_returned = NULL;

    if(size < max_size) {
        for (size_t i = 0; i < memmgr_count; i++) {
            if (!mock_memmgr.used[i]) {
                //_gt_test_print("allocation at index %lu\n", i);

                size_t ints_allocated = (size - sizeof(_choco_arraylist_header)) / sizeof(int);
                _choco_arraylist_allocator allocator = { .allocate = mock_alloc, .deallocate = mock_dealloc };
                ptr_returned = &mock_memmgr.mocks[i];
                init_new_mock(&mock_memmgr.mocks[i], ints_allocated, 0, allocator);
                mock_memmgr.used[i] = 1;
                mock_memmgr.a_last_ptr = ptr_returned;
                mock_memmgr.a_last_req_size = size;
                break;
            }
        }
    }

    //_gt_test_print("%lu <? %lu\n", size, max_size);

    mock_memmgr.a_last_req_size = size;
    mock_memmgr.a_last_ptr = ptr_returned;
    return ptr_returned;
}

static void init_mock_memmgr(void) {
    mock_memmgr = (struct _memmgr) {
        .mocks = { 0 },
        .used = { 0 },
        .a_last_req_size = 0,
        .a_last_ptr = NULL,
        .d_last_ptr = NULL
    };
}

static _choco_arraylist_allocator init_invalid_allocator(void) {
     _choco_arraylist_allocator allocator = {
        .allocate = NULL,
        .deallocate = NULL
    };
    return allocator;
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
    size_t desired = 10;
    _mock mock;
    init_new_mock(&mock, 10, 0, init_new_allocator());

    // act
    _choco_arraylist_header* header = _choco_arraylist_get_header(mock.data);

    // assert
    _gt_test_ptr_eq(header, &mock.header);
    _gt_test_int_eq(header->allocated, desired);
    _gt_test_int_eq(header->size, sizeof(int));
    _gt_test_ptr_eq(header->data, &mock.data[0]);
    _gt_passed();
}

_gt_test(_choco_arraylist_length, )
{
    // arrange
    size_t used = 3;
    _mock mock;
    init_new_mock(&mock, 10, used, init_new_allocator());

    // act
    size_t length = _choco_arraylist_length(mock.data);

    // assert
    _gt_test_int_eq(length, used);
    _gt_passed();
}

_gt_test(_choco_arraylist_element_size, )
{
    // arrange
    _mock mock;
    init_new_mock(&mock, 10, 0, init_new_allocator());

    // act
    size_t size = _choco_arraylist_element_size(mock.data);

    // assert
    _gt_test_int_eq(size, sizeof(int));
    _gt_passed();
}

_gt_test(_choco_arraylist_sizeof, )
{
    // arrange
    _mock mock;
    init_new_mock(&mock, 10, 0, init_new_allocator());

    // act
    size_t physical_size = _choco_arraylist_sizeof(mock.data);

    // assert
    _gt_test_int_eq(physical_size, sizeof(mock_arrlist));
    _gt_passed();
}

_gt_test(_choco_arraylist_at, )
{
    // arrange
    size_t used = 4;
    size_t index = 2;
    size_t invalid_index = 7;
    _mock mock;
    init_new_mock(&mock, 10, used, init_new_allocator());
    int* valid_ptr = &mock.data[index];
    int* invalid_ptr = NULL;

    // act
    int* valid_result = _choco_arraylist_at(mock.data, index);
    int* invalid_result = _choco_arraylist_at(mock.data, invalid_index);

    // assert
    _gt_test_ptr_eq(valid_result, valid_ptr);
    _gt_test_ptr_eq(invalid_result, invalid_ptr);
    _gt_passed();
}

_gt_test(_choco_arraylist_swap, )
{
    // arrange
    size_t used = 4;
    size_t indexA = 1;
    size_t indexB = 2;
    _mock mock;
    init_new_mock(&mock, 10, used, init_new_allocator());
    int valueA = mock.data[indexA];
    int valueB = mock.data[indexB];

    // act
    _choco_arraylist_result result = _choco_arraylist_swap(mock.data, indexA, indexB);

    // assert
    _gt_test_int_eq(result, _CHOCO_ARRAYLIST_RESULT_OK);
    _gt_test_int_eq(mock.data[indexB], valueA);
    _gt_test_int_eq(mock.data[indexA], valueB);
    _gt_passed();
}

_gt_test(_choco_arraylist_swap, invalid_index)
{
    // arrange
    size_t used = 4;
    size_t indexA = 1;
    size_t indexB = 6;
    _mock mock;
    init_new_mock(&mock, 10, used, init_new_allocator());
    int valueA = mock.data[indexA];

    // act
    _choco_arraylist_result result = _choco_arraylist_swap(mock_arrlist.data, indexA, indexB);

    // assert
    _gt_test_int_eq(result, _CHOCO_ARRAYLIST_RESULT_ERROR);
    _gt_test_int_eq(mock.data[indexA], valueA); // stays unchanged.
    _gt_passed();
}

_gt_test(_choco_arraylist_remove, )
{
    // arrange
    size_t used = 4;
    _mock mock;
    init_new_mock(&mock, 10, used, init_new_allocator());

    // act
    _choco_arraylist_remove(mock.data);

    // assert
    _gt_test_int_eq(mock.header.used, used - 1);
    _gt_passed();
}

_gt_test(_choco_arraylist_remove, when_empty)
{
    // arrange
    size_t used = 0;
    _mock mock;
    init_new_mock(&mock, 10, used, init_new_allocator());

    // act
    _choco_arraylist_remove(mock.data);

    // assert
    _gt_test_int_eq(mock.header.used, 0);
    _gt_passed();
}

_gt_test(_choco_arraylist_create, )
{
    // arrange
    init_mock_memmgr();
    _choco_arraylist_allocator allocator = init_new_allocator();
    _choco_arraylist_allocator invalid_allocator = init_invalid_allocator();
    const size_t desired = 1;
    const size_t size = sizeof(int);
    const size_t req_size = sizeof(_choco_arraylist_header) + desired * size;

    // act
    _choco_arraylist arrlist = _choco_arraylist_create(allocator, size, desired);

    // assert
    _choco_arraylist_header* header = ((_choco_arraylist_header*)arrlist) - 1;
    _gt_test_ptr_neq(arrlist, NULL);
    _gt_test_int_eq(header->allocated, desired);
    _gt_test_int_eq(header->used, 0);
    _gt_test_int_eq(header->size, size);
    _gt_test_int_eq(mock_memmgr.a_last_req_size, req_size);
    _gt_test_ptr_eq(mock_memmgr.a_last_ptr, header);
    _gt_passed();
}

_gt_test(_choco_arraylist_create, invalid_allocator) {
    // arrange
    init_mock_memmgr();
    _choco_arraylist_allocator invalid_allocator = init_invalid_allocator();
    const size_t desired = 1;
    const size_t size = sizeof(int);

    // act
    _choco_arraylist fail = _choco_arraylist_create(invalid_allocator, size, desired);

    // assert
    _gt_test_ptr_eq(fail, NULL);
    _gt_test_ptr_eq(mock_memmgr.a_last_ptr, NULL);
    _gt_passed();
}

_gt_test(_choco_arraylist_create, mem_alloc_failed)
{
    // arrange
    init_mock_memmgr();
    _choco_arraylist_allocator allocator = init_new_allocator();
    size_t desired = 11;
    size_t size = sizeof(int);
    size_t req_size = sizeof(_choco_arraylist_header) + desired * size;

    // act
    _choco_arraylist arrlist = _choco_arraylist_create(allocator, size, desired);

    // assert
    _gt_test_ptr_eq(arrlist, NULL);
    _gt_test_int_eq(mock_memmgr.a_last_req_size, req_size);
    _gt_passed();
}

_gt_test(_choco_arraylist_resize, )
{
    // arrange
    const size_t desired = 5;
    const size_t size = sizeof(int);

    init_mock_memmgr();
    _choco_arraylist_allocator allocator = init_new_allocator();
    _choco_arraylist arrlist = _choco_arraylist_create(allocator, size, 1);

    // act
    _choco_arraylist result = _choco_arraylist_resize(arrlist, desired);

    // assert
    void* last_allocated_ptr = mock_memmgr.a_last_ptr;
    _choco_arraylist_header* result_header = result - sizeof(_choco_arraylist_header);

    _gt_test_ptr_neq(result, arrlist)
    _gt_test_ptr_eq(result_header, last_allocated_ptr);
    _gt_test_int_eq(result_header->allocated, desired);
    _gt_passed();
}

_gt_test(_choco_arraylist_resize, mem_alloc_failed)
{
    // arrange
    const size_t desired = 13;
    const size_t size = sizeof(int);

    init_mock_memmgr();
    _choco_arraylist_allocator allocator = init_new_allocator();
    _choco_arraylist arrlist = _choco_arraylist_create(allocator, size, 2);

    // act
    _choco_arraylist result = _choco_arraylist_resize(arrlist, desired);

    // assert
    _choco_arraylist_header* header = result - sizeof(_choco_arraylist_header);

    _gt_test_ptr_eq(result, arrlist);
    _gt_test_int_eq(header->allocated, 2);
    _gt_passed();
}

_gt_test(_choco_arraylist_resize, alloc_invalid)
{
    // arrange
    const size_t desired = 6;
    const size_t size = sizeof(int);

    init_mock_memmgr();
    _choco_arraylist_allocator allocator = init_new_allocator();
    _choco_arraylist arrlist = _choco_arraylist_create(allocator, size, 2);
    _choco_arraylist_header* initial_header = arrlist - sizeof(_choco_arraylist_header);
    initial_header->allocator = init_invalid_allocator();

    // act
    _choco_arraylist result = _choco_arraylist_resize(arrlist, desired);

    // assert
    _choco_arraylist_header* header = result - sizeof(_choco_arraylist_header);

    _gt_test_ptr_eq(result, arrlist);
    _gt_test_int_eq(header->allocated, 2);
    _gt_passed();
}

_gt_test(_choco_arraylist_is_full, no)
{
    // arrange
    size_t alloc = 7;
    size_t used = 3;
    _mock mock;
    init_new_mock(&mock, alloc, used, init_new_allocator());

    // act
    _choco_arraylist_result result = _choco_arraylist_is_full(mock.data);

    // assert
    _gt_test_int_eq(result, _CHOCO_ARRAYLIST_RESULT_NO);
    _gt_passed();
}

_gt_test(_choco_arraylist_is_full, yes)
{
    // arrange
    size_t alloc = 7;
    size_t used = 7;
    _mock mock;
    init_new_mock(&mock, alloc, used, init_new_allocator());

    // act
    _choco_arraylist_result result = _choco_arraylist_is_full(mock.data);

    // assert
    _gt_test_int_eq(result, _CHOCO_ARRAYLIST_RESULT_YES);
    _gt_passed();
}

_gt_test(_choco_arraylist_add, )
{
    // arrange
    init_mock_memmgr();
    size_t desired = 4;
    size_t size = sizeof(int);
    size_t expected_used = 1;
    _choco_arraylist_allocator allocator = init_new_allocator();
    _choco_arraylist initial = _choco_arraylist_create(allocator, size, desired);
    _choco_arraylist_header* initial_header = initial - sizeof(_choco_arraylist_header);

    // act
    _choco_arraylist result = _choco_arraylist_add(initial);

    // assert
    _choco_arraylist_header* result_header = ((_choco_arraylist_header*) result) - 1;
    void* last_allocated_ptr = mock_memmgr.a_last_ptr;

    _gt_test_ptr_eq(last_allocated_ptr, initial_header);
    _gt_test_ptr_eq(initial, result);
    _gt_test_int_eq(result_header->used, expected_used);
    _gt_passed();
}

_gt_test(_choco_arraylist_add, when_full)
{
    // arrange
    init_mock_memmgr();
    size_t desired = 3;
    size_t size = sizeof(int);
    size_t expected_used = 4;
    _choco_arraylist_allocator allocator = init_new_allocator();
    _choco_arraylist initial = _choco_arraylist_create(allocator, size, desired);
    _choco_arraylist_header* initial_header = initial - sizeof(_choco_arraylist_header);
    initial_header->used = desired; // makes the arraylist full.

    // act
    _choco_arraylist result = _choco_arraylist_add(initial);

    // assert
    _choco_arraylist_header* result_header = ((_choco_arraylist_header*) result) - 1;
    void* last_allocated_ptr = mock_memmgr.a_last_ptr;
    _gt_test_ptr_eq(last_allocated_ptr, result_header);
    _gt_test_ptr_neq(initial, result);
    _gt_test_int_eq(result_header->used, expected_used);
    _gt_passed();
}

_gt_test(_choco_arraylist_add, alloc_fail)
{
    // arrange
    init_mock_memmgr();
    size_t desired = 3;
    size_t size = sizeof(int);
    _choco_arraylist_allocator allocator = init_new_allocator();
    _choco_arraylist initial = _choco_arraylist_create(allocator, size, desired);
    _choco_arraylist_header* initial_header = initial - sizeof(_choco_arraylist_header);
    initial_header->allocator = init_invalid_allocator(); // makes allocator invalid

    // act
    _choco_arraylist result = _choco_arraylist_add(initial);

    // assert
    _gt_test_ptr_eq(initial, result);
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
    _gt_run(_choco_arraylist_create, invalid_allocator);
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
