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

#pragma pack(push, 1)

typedef struct _choco_arraylist_test_struct _choco_arraylist_test_struct;
struct _choco_arraylist_test_struct { 
    _choco_arraylist_header header; 
    int data[6];
};

#pragma pack(pop)

static _choco_arraylist_test_struct mock_arrlist;
static void init_test_struct(void) {
    mock_arrlist = (_choco_arraylist_test_struct) {
        .header = {
            .allocator = {.allocate = NULL, .dealloate = NULL},
            .allocated = sizeof(mock_arrlist.data) / sizeof(mock_arrlist.data[0]), 
            .used = 0, 
            .size = sizeof(int), 
            .data = mock_arrlist.data
        }, 
        .data = {0, 1, 2, 3, 4, 5}
    };
}

static unsigned mock_alloc_last_req_sz = 0;
static int mock_alloc_last_req_res = 0;
static void* mock_alloc(void* self, unsigned size) {
    static const unsigned initial_allocated = 2;
    static const unsigned initial_size = sizeof(mock_arrlist.header) + sizeof(int) * initial_allocated;
    static const unsigned extended_allocated = 6;
    static const unsigned extended_size = sizeof(mock_arrlist.header) + sizeof(int) * extended_allocated;

    mock_alloc_last_req_sz = size;

    if(size <= initial_size) {
        init_test_struct();
        mock_alloc_last_req_res = 1;
        return &mock_arrlist;
    }

    if(size <= extended_size) {
        mock_alloc_last_req_res = 1;
        return &mock_arrlist;
    }

    mock_alloc_last_req_res = 0;
    return NULL;
}

static void* mock_dealloc_last_ptr = NULL;
static void mock_dealloc(void* self, void* ptr) {
    mock_dealloc_last_ptr = ptr;
}

static _choco_arraylist_allocator get_test_allocator(void) {
    _choco_arraylist_allocator allocator = {
        .allocate = mock_alloc,
        .dealloate = mock_dealloc
    };
    return allocator;
}

_gt_test(_choco_arraylist_get_header, ) {
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

_gt_test(_choco_arraylist_length, ) {
    // arrange
    init_test_struct();
    mock_arrlist.header.used = 3;

    // act
    unsigned length = _choco_arraylist_length(mock_arrlist.data);

    // assert
    _gt_test_int_eq(length, 3);
    _gt_passed();
}

_gt_test(_choco_arraylist_element_size, ) {
    // arrange
    init_test_struct();

    // act
    unsigned size = _choco_arraylist_element_size(mock_arrlist.data);

    // assert
    _gt_test_int_eq(size, sizeof(int));
    _gt_passed();
}

_gt_test(_choco_arraylist_sizeof, ) {
    // arrange
    init_test_struct();

    // act
    unsigned physical_size = _choco_arraylist_sizeof(mock_arrlist.data);

    // assert
    _gt_test_int_eq(physical_size, sizeof(mock_arrlist));
    _gt_passed();
}

_gt_test(_choco_arraylist_at, ) {
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

_gt_test(_choco_arraylist_swap, ) {
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

_gt_test(_choco_arraylist_swap, invalid_index) {
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

_gt_test(_choco_arraylist_remove, ) {
    // arrange
    init_test_struct();
    mock_arrlist.header.used = 4;
    unsigned old_used = mock_arrlist.header.used;
    
    // act
    _choco_arraylist_remove(mock_arrlist.data);

    // assert
    _gt_test_int_eq(mock_arrlist.header.used, old_used - 1);
    _gt_passed();
}

_gt_test(_choco_arraylist_remove, when_empty) {
    // arrange
    init_test_struct();
    
    // act
    _choco_arraylist_remove(mock_arrlist.data);

    // assert
    _gt_test_int_eq(mock_arrlist.header.used, 0);
    _gt_passed();
}

_gt_test(_choco_arraylist_create_x, ) {
    // arrange
    _choco_arraylist_allocator allocator = get_test_allocator();
    unsigned size = sizeof(int);
    unsigned alloc = 1;

    // act
    _choco_arraylist arrlist = _choco_arraylist_create_x(allocator, size, alloc);

    // assert
    _gt_test_ptr_neq(arrlist, NULL);
    _gt_test_ptr_eq(arrlist, mock_arrlist.data);
    _gt_test_int_eq(mock_arrlist.header.allocated, alloc);
    _gt_test_int_eq(mock_alloc_last_req_res, 1);
    _gt_passed();
}

_gt_test(_choco_arraylist_create_x, mem_alloc_failed) {
    // arrange
    _choco_arraylist_allocator allocator = get_test_allocator();
    unsigned size = sizeof(int);
    unsigned alloc = 10;
    unsigned physical_size = sizeof(_choco_arraylist_header) + sizeof(int) * alloc;

    // act
    _choco_arraylist arrlist = _choco_arraylist_create_x(allocator, size, alloc);

    // assert
    _gt_test_ptr_eq(arrlist, NULL);
    _gt_test_int_eq(mock_alloc_last_req_sz, physical_size);
    _gt_test_int_eq(mock_alloc_last_req_res, 0);
    _gt_passed();
}

_gt_test(_choco_arraylist_resize, ) {
    // arrange
    init_test_struct();
    mock_arrlist.header.allocated = 2;
    mock_arrlist.header.allocator = get_test_allocator();
    unsigned to_alloc = 5;
    unsigned req_size = sizeof(mock_arrlist.header) + sizeof(int) * to_alloc;
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

_gt_test(_choco_arraylist_resize, mem_alloc_failed) {
    // arrange
    init_test_struct();
    mock_arrlist.header.allocated = 2;
    mock_arrlist.header.allocator = get_test_allocator();
    unsigned to_alloc = 10;
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

_gt_test(_choco_arraylist_resize, alloc_invalid) {
    // arrange
    init_test_struct();
    mock_arrlist.header.allocated = 2;
    mock_arrlist.header.allocator = (_choco_arraylist_allocator){0};
    mock_alloc_last_req_sz = 0;
    unsigned to_alloc = 5;
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

_gt_test(_choco_arraylist_is_full, no) {
    // arrange
    init_test_struct();
    mock_arrlist.header.used = 2;

    // act
    int result = _choco_arraylist_is_full(mock_arrlist.data);

    // assert
    _gt_test_int_eq(result, 0);
    _gt_passed();
}

_gt_test(_choco_arraylist_is_full, yes) {
    // arrange
    init_test_struct();
    mock_arrlist.header.used = 10;

    // act
    int result = _choco_arraylist_is_full(mock_arrlist.data);

    // assert
    _gt_test_int_eq(result, 1);
    _gt_passed();
}

_gt_test(_choco_arraylist_add, ) {
    // arrange
    unsigned alloc = 2;
    unsigned used = 0;
    unsigned used_after_add = used + 1;
    init_test_struct();
    _choco_arraylist_header* header = &mock_arrlist.header;
    header->allocated = alloc;
    header->used = used;
    header->allocator = get_test_allocator();
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

_gt_test(_choco_arraylist_add, when_full) {
    // arrange
    init_test_struct();
    _choco_arraylist_header* header = &mock_arrlist.header;
    _choco_arraylist arrlist = mock_arrlist.data;
    unsigned alloc = 2;
    unsigned used = 2;
    unsigned used_after_add = used + 1;
    unsigned alloc_after_add = (alloc + 1) * 2;
    unsigned request_size = sizeof(int) * alloc_after_add + sizeof(*header);
    header->allocated = alloc;
    header->used = used;
    header->allocator = get_test_allocator();
    
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

_gt_test(_choco_arraylist_add, alloc_fail) {
    // arrange
    init_test_struct();
    _choco_arraylist_header* header = &mock_arrlist.header;
    _choco_arraylist arrlist = mock_arrlist.data;
    unsigned alloc = 6;
    unsigned used = 6;
    unsigned alloc_after_add = (alloc + 1) * 2;
    unsigned request_size = sizeof(int) * alloc_after_add + sizeof(*header);
    header->allocated = alloc;
    header->used = used;
    header->allocator = get_test_allocator();
    
    // act
    _choco_arraylist result = _choco_arraylist_add(mock_arrlist.data);

    // assert
    _gt_test_ptr_eq(result, NULL);
    _gt_test_int_eq(mock_alloc_last_req_sz, request_size);
    _gt_test_ptr_eq(mock_dealloc_last_ptr, header);
    _gt_passed();
}

void _choco_arraylist_test(void) {
    _gt_run(_choco_arraylist_get_header, );
    _gt_run(_choco_arraylist_length, );
    _gt_run(_choco_arraylist_element_size, );
    _gt_run(_choco_arraylist_sizeof, );
    _gt_run(_choco_arraylist_at, );
    _gt_run(_choco_arraylist_swap, );
    _gt_run(_choco_arraylist_swap, invalid_index);
    _gt_run(_choco_arraylist_remove, );
    _gt_run(_choco_arraylist_remove, when_empty);
    _gt_run(_choco_arraylist_create_x, );
    _gt_run(_choco_arraylist_create_x, mem_alloc_failed);
    _gt_run(_choco_arraylist_resize, );
    _gt_run(_choco_arraylist_resize, mem_alloc_failed);
    _gt_run(_choco_arraylist_resize, alloc_invalid);
    _gt_run(_choco_arraylist_is_full, no);
    _gt_run(_choco_arraylist_is_full, yes);
    _gt_run(_choco_arraylist_add, );
    _gt_run(_choco_arraylist_add, when_full);
    _gt_run(_choco_arraylist_add, alloc_fail);
}
