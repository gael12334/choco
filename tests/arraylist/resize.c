/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "../arraylist.h"

#define mock_memmgr _choco_arraylist_mock_memmgr
#define mock_alloc _choco_arraylist_mock_alloc
#define mock_arraylist _choco_arraylist_mock
#define mock_dealloc _choco_arraylist_mock_dealloc
#define mock_alloc _choco_arraylist_mock_alloc

/*
 * Create and Destroy functions hooks.
 */

static struct create_data {
    _choco_arraylist_obj output;
    _choco_arraylist_result result;
    size_t size;
    size_t called;
} create;
static _choco_arraylist_obj create_hook(_choco_arraylist_memmgr memory, size_t used, size_t size, _choco_arraylist_result* out)
{
    create.called++;
    create.size = size;
    *out = create.result;
    return create.output;
}

static struct destroy_data {
    _choco_arraylist_result result;
    _choco_arraylist_obj destructed;
    size_t called;
} destroy;
static void destroy_hook(_choco_arraylist_obj self, _choco_arraylist_result* out)
{
    destroy.called++;
    destroy.destructed = self;
    *out = destroy.result;
}

/*
 * Hook setup functions.
 */

static void* destroy_backup;
static void* create_backup;
static void init(void)
{
    destroy_backup = _choco_arraylist.destroy; // backup of original functions.
    create_backup = _choco_arraylist.create;
    _choco_arraylist.destroy = destroy_hook; // overriding with hooks.
    _choco_arraylist.create = create_hook;
    create = (struct create_data) { 0 };
    destroy = (struct destroy_data) { 0 };
}
static void cleanup(void)
{
    _choco_arraylist.destroy = destroy_backup; // placing back original functions
    _choco_arraylist.create = create_backup;
}

/*
 * Mocks
 */

#define first_size 5
static struct mock_arraylist first_mock = {
    .memory = { 0 },
    .length = first_size,
    .size = first_size,
    .units = sizeof(size_t),
    .list = { 0, 1, 2, 3, 4 }
};

#define second_size 10
static struct mock_arraylist second_mock = {
    .memory = { 0 },
    .length = first_size,
    .size = second_size,
    .units = sizeof(size_t),
    .list = { 0, 1, 2, 3, 4 }
};

/*
 * Tests
 */

_gt_test(resize, )
{
    // arrange
    init();
    _choco_arraylist_obj first_obj = { .ptr = first_mock.list };
    _choco_arraylist_obj second_obj = { .ptr = second_mock.list };
    create.output = second_obj;
    create.result = _CHOCO_ARRAYLIST_OK;
    destroy.result = _CHOCO_ARRAYLIST_OK;

    // act
    _choco_arraylist_result result;
    _choco_arraylist_obj obj = _choco_arraylist.resize(first_obj, second_size, &result);

    // assert
    _gt_test_int_eq(result, _CHOCO_ARRAYLIST_OK);
    _gt_test_int_eq(create.size, second_size);
    _gt_test_ptr_eq(obj.ptr, second_obj.ptr);
    _gt_test_ptr_eq(destroy.destructed.ptr, first_obj.ptr);
    _gt_passed();
    cleanup();
}

_gt_test(resize, identical_size)
{
    // arrange
    _choco_arraylist_obj first_obj = { .ptr = first_mock.list };

    // act
    _choco_arraylist_result result;
    _choco_arraylist_obj obj = _choco_arraylist.resize(first_obj, first_size, &result);

    // assert
    _gt_test_int_eq(result, _CHOCO_ARRAYLIST_OK);
    _gt_test_ptr_eq(obj.ptr, first_obj.ptr);
    _gt_passed();
}

void _choco_arraylist_test_resize(void)
{
    init();
    _gt_run(resize, );
    _gt_run(resize, identical_size);
    cleanup();
}
