/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "../src/arraylist.h"
#include "../src/gt/test.h"
#include <string.h>

#define mock_memmgr _choco_arraylist_mock_memmgr
#define mock_alloc _choco_arraylist_mock_alloc
#define mock_arraylist _choco_arraylist_mock
#define mock_dealloc _choco_arraylist_mock_dealloc
#define mock_alloc _choco_arraylist_mock_alloc

#define MOCK_MEMMGR_MAX_CALLS 5
struct mock_memmgr {
    struct mock_alloc {
        void* return_pointer;
        size_t recieved_size;
        _choco_arraylist_result out_value;
    } alloc_values[MOCK_MEMMGR_MAX_CALLS];

    struct mock_dealloc {
        void* recieved_pointer;
        _choco_arraylist_result out_value;
    } dealloc_values[MOCK_MEMMGR_MAX_CALLS];

    int alloc_calls;
    int dealloc_calls;
};

#define MOCK_ARRAYLIST_MAX_SIZE 10
struct mock_arraylist {
    _choco_arraylist_memmgr memory;
    size_t units;
    size_t length;
    size_t size;
    size_t list[MOCK_ARRAYLIST_MAX_SIZE];
};

void mock_dealloc(void* obj, void* ptr, _choco_arraylist_result* out);
void* mock_alloc(void* obj, size_t size, _choco_arraylist_result* out);

void _choco_arraylist_test_create(void);
void _choco_arraylist_test_clone(void);
void _choco_arraylist_test(void);

#undef mock_memmgr
#undef mock_alloc
#undef mock_arraylist
#undef mock_dealloc
#undef mock_alloc
