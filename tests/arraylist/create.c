/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "../arraylist.h"

#define mock_memmgr _choco_arraylist_mock_memmgr
#define mock_alloc _choco_arraylist_mock_alloc
#define mock_arraylist _choco_arraylist_mock
#define mock_dealloc _choco_arraylist_mock_dealloc
#define mock_alloc _choco_arraylist_mock_alloc

_gt_test(create, )
{
    // arrange
    size_t size = 10;
    size_t units = sizeof(size_t);
    size_t header_size = sizeof(_choco_arraylist_header);
    size_t buffer_size = header_size + units * size;
    char buffer[buffer_size];
    struct _choco_arraylist_mock_memmgr mock = {
        .alloc_calls = 0,
        .alloc_values = {
            (struct mock_alloc) {
                .return_pointer = buffer,
                .out_value = _CHOCO_ARRAYLIST_OK,
            } }
    };

    _choco_memmgr_obj memmgr = {
        .obj = &mock,
        .alloc = mock_alloc,
        .dealloc = mock_dealloc
    };

    // act
    _choco_arraylist_result result;
    _choco_arraylist_obj obj = _choco_arraylist.create(memmgr, units, size, &result);

    // assert
    size_t recieved_size = mock.alloc_values[0].recieved_size;

    _gt_test_int_eq(result, _CHOCO_ARRAYLIST_OK);
    _gt_test_ptr_eq(obj.ptr - header_size, buffer);
    _gt_test_int_eq(mock.alloc_calls, 1);
    _gt_test_int_eq(recieved_size, buffer_size);
    _gt_passed();
}

_gt_test(create, inv_memmgr)
{
    // arrange
    size_t size = 10;
    size_t units = sizeof(size_t);

    _choco_memmgr_obj memmgr = {
        .obj = NULL,
        .alloc = NULL,
        .dealloc = NULL
    };

    // act
    _choco_arraylist_result result;
    _choco_arraylist_obj obj = _choco_arraylist.create(memmgr, units, size, &result);

    // assert
    _gt_test_int_eq(result, _CHOCO_ARRAYLIST_INV_MEMMGR);
    _gt_test_ptr_eq(obj.ptr, NULL);
    _gt_passed();
}

_gt_test(create, err_alloc)
{
    // arrange
    size_t size = 10;
    size_t units = sizeof(int);
    size_t header_size = sizeof(_choco_arraylist_header);
    size_t buffer_size = header_size + units * size;
    struct _choco_arraylist_mock_memmgr mock = {
        .alloc_calls = 0,
        .alloc_values = {
            (struct mock_alloc) {
                .return_pointer = NULL,
                .out_value = _CHOCO_ARRAYLIST_ERR_ALLOC,
            } }
    };

    _choco_memmgr_obj memmgr = {
        .obj = &mock,
        .alloc = mock_alloc,
        .dealloc = mock_dealloc
    };

    // act
    _choco_arraylist_result result;
    _choco_arraylist_obj obj = _choco_arraylist.create(memmgr, units, size, &result);

    // assert
    size_t recieved_size = mock.alloc_values[0].recieved_size;

    _gt_test_int_eq(result, _CHOCO_ARRAYLIST_ERR_ALLOC);
    _gt_test_ptr_eq(obj.ptr, NULL);
    _gt_test_int_eq(mock.alloc_calls, 1);
    _gt_test_int_eq(recieved_size, buffer_size);
    _gt_passed();
}

void _choco_arraylist_test_create(void)
{
    _gt_run(create, );
    _gt_run(create, inv_memmgr);
    _gt_run(create, err_alloc);
}
