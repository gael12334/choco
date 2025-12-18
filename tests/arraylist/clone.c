/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "../arraylist_test.h"

#define mock_memmgr _choco_arraylist_mock_memmgr
#define mock_alloc _choco_arraylist_mock_alloc
#define mock_arraylist _choco_arraylist_mock
#define mock_dealloc _choco_arraylist_mock_dealloc
#define mock_alloc _choco_arraylist_mock_alloc

struct mock_arraylist clone_other_mock = {
    .memory = { 0 },
    .length = MOCK_ARRAYLIST_MAX_SIZE / 2,
    .size = MOCK_ARRAYLIST_MAX_SIZE,
    .units = sizeof(size_t),
    .list = { 0, 1, 2, 3, 4 }
};

_gt_test(clone, )
{
    // arrange
    struct mock_arraylist result_mock;
    struct _choco_arraylist_mock_memmgr memmgr_mock = {
        .alloc_calls = 0,
        .alloc_values = {
            (struct mock_alloc) {
                .return_pointer = &result_mock,
                .out_value = _CHOCO_ARRAYLIST_OK,
            } }
    };

    _choco_arraylist_obj other = { .ptr = clone_other_mock.list };
    _choco_arraylist_memmgr memmgr = {
        .obj = &memmgr_mock,
        .alloc = mock_alloc,
        .dealloc = mock_dealloc
    };

    // act

    _choco_arraylist_result result;
    _choco_arraylist_obj obj = _choco_arraylist.clone(memmgr, other, 0, clone_other_mock.length, &result);

    // assert
    size_t recieved_size = memmgr_mock.alloc_values[0].recieved_size;

    _gt_test_int_eq(result, _CHOCO_ARRAYLIST_OK);
    _gt_test_ptr_eq(obj.ptr, result_mock.list);
    _gt_test_int_eq(memmgr_mock.alloc_calls, 1);
    _gt_test_int_eq(result_mock.size, clone_other_mock.size);
    _gt_test_int_eq(result_mock.length, clone_other_mock.length);
    _gt_passed();
}

_gt_test(clone, inv_arrlist)
{
    // arrange
    _choco_arraylist_memmgr memmgr = { 0 };
    _choco_arraylist_obj other = { .ptr = NULL };

    // act
    _choco_arraylist_result result;
    _choco_arraylist_obj obj = _choco_arraylist.clone(memmgr, other, 1, 1, &result);

    // assert
    _gt_test_int_eq(result, _CHOCO_ARRAYLIST_INV_OTHER);
    _gt_test_ptr_eq(obj.ptr, NULL);
    _gt_passed();
}

_gt_test(clone, inv_range)
{
    // arrange
    _choco_arraylist_memmgr memmgr = { 0 };
    _choco_arraylist_obj other = { .ptr = clone_other_mock.list };

    // act
    _choco_arraylist_result result1, result2;
    _choco_arraylist_obj obj1 = _choco_arraylist.clone(memmgr, other, 7, 1, &result1);
    _choco_arraylist_obj obj2 = _choco_arraylist.clone(memmgr, other, 3, 16, &result2);

    // assert
    _gt_test_int_eq(result1, _CHOCO_ARRAYLIST_INV_RANGE);
    _gt_test_int_eq(result2, _CHOCO_ARRAYLIST_INV_RANGE);
    _gt_test_ptr_eq(obj1.ptr, NULL);
    _gt_test_ptr_eq(obj2.ptr, NULL);
    _gt_passed();
}

void _choco_arraylist_test_clone(void)
{
    _gt_run(clone, );
    _gt_run(clone, inv_arrlist);
    _gt_run(clone, inv_range);
}
