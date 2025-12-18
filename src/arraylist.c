/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "arraylist.h"
#include <string.h>

#define arraylist _choco_arraylist_obj
#define result _choco_arraylist_result
#define memmgr _choco_arraylist_memmgr
#define interface _choco_arraylist
#define header_size _choco_arraylist_header_size

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

/*
 * Internal structs
 */

typedef struct header {
    memmgr memory;
    size_t units;
    size_t length;
    size_t size;
} header;

/*
 * Private functions
 */

static int is_memmgr_valid(memmgr memory)
{
    return memory.alloc != NULL && memory.dealloc != NULL;
}

static int is_arraylist_valid(arraylist obj)
{
    return obj.ptr != NULL;
}

static int is_index_valid(header* h, size_t index)
{
    return index < h->length;
}

static int is_range_valid(header* h, size_t index, size_t size)
{
    return index < h->length && (index + size) <= h->length;
}

static int is_units_valid(header* h, size_t units)
{
    return h->units == units;
}

static int is_alloc_successful(void* v, result status)
{
    return v != NULL && status == _CHOCO_ARRAYLIST_OK;
}

static header* get_header(arraylist obj)
{
    return obj.ptr - sizeof(header);
}

static void* get_element(arraylist obj, size_t units, size_t index)
{
    return obj.ptr + units * index;
}

static size_t get_data_size(size_t units, size_t size)
{
    return size * units;
}

static size_t get_physical_size(header* h)
{
    return sizeof(*h) + get_data_size(h->size, h->units);
}

static arraylist get_arraylist(header* h)
{
    return (arraylist) { .ptr = h + 1 };
}

static arraylist get_null_arraylist(void)
{
    return (arraylist) { .ptr = NULL };
}

static void set_length(header* h, size_t length)
{
    h->length = length;
}

static void set_result(result* out, result r)
{
    if (out != NULL) {
        *out = r;
    }
}

static void set_data_from_arraylist(arraylist self, size_t s_index, arraylist other, size_t o_index, size_t units, size_t size)
{
    void* s_element = get_element(self, units, s_index);
    void* o_element = get_element(other, units, o_index);
    memcpy(s_element, o_element, get_data_size(units, size));
}

/*
 * Public functions
 */

static arraylist create(memmgr memory, size_t units, size_t size, result* out)
{
    if (!is_memmgr_valid(memory)) {
        set_result(out, _CHOCO_ARRAYLIST_INV_MEMMGR);
        return get_null_arraylist();
    }

    header h = {
        .length = 0,
        .size = size,
        .units = units,
        .memory = memory
    };

    result status;
    size_t needed = get_physical_size(&h);
    header* ptr = memory.alloc(memory.obj, needed, &status);

    if (!is_alloc_successful(ptr, status)) {
        set_result(out, _CHOCO_ARRAYLIST_ERR_ALLOC);
        return get_null_arraylist();
    }

    *ptr = h;
    set_result(out, _CHOCO_ARRAYLIST_OK);
    return get_arraylist(ptr);
}

static arraylist clone(memmgr memory, arraylist other, size_t index, size_t size, result* out)
{
    if (!is_arraylist_valid(other)) {
        set_result(out, _CHOCO_ARRAYLIST_INV_OTHER);
        return get_null_arraylist();
    }

    header* other_header = get_header(other);
    if (!is_range_valid(other_header, index, size)) {
        set_result(out, _CHOCO_ARRAYLIST_INV_RANGE);
        return get_null_arraylist();
    }

    result status;
    arraylist self = interface.create(memory, other_header->units, other_header->size, &status);
    if (!is_alloc_successful(self.ptr, status)) {
        set_result(out, status);
        return get_null_arraylist();
    }

    set_length(get_header(self), size);
    set_data_from_arraylist(self, 0, other, index, other_header->units, size);
    set_result(out, _CHOCO_ARRAYLIST_OK);
    return self;
}

static arraylist push(arraylist self, void* ref, size_t units, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return get_null_arraylist();
}

static arraylist pull(arraylist self, arraylist other, size_t index, size_t size, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return get_null_arraylist();
}

static arraylist reduce(arraylist self, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return get_null_arraylist();
}

static size_t size_of(arraylist self, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return 0;
}

static size_t length(arraylist self, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return 0;
}

static size_t units(arraylist self, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return 0;
}

static size_t size(arraylist self, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return 0;
}

static void* remove(arraylist self, void* ref, size_t units, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return NULL;
}

static void* first(arraylist self, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return NULL;
}

static void* last(arraylist self, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return NULL;
}

static void* at(arraylist self, size_t index, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return NULL;
}

static void destroy(arraylist self, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
}

static void fill(arraylist self, void* ref, size_t units, size_t index, size_t size, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
}

static void copy(arraylist self, arraylist other, size_t index, size_t size, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
}

static void clear(arraylist self, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
}

static void swap(arraylist self, size_t index, size_t other, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
}

static void flip(arraylist self, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
}

static void sort(arraylist self, size_t units, size_t offset, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
}

static int is_empty(arraylist self, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return 0;
}

static int equals(arraylist self, arraylist other, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return 0;
}

static int pop(arraylist self, size_t size, result* out)
{
    set_result(out, _CHOCO_ARRAYLIST_NOT_IMPL);
    return 0;
}

const size_t header_size = sizeof(header);

struct interface interface = {
    .at = at,
    .clear = clear,
    .clone = clone,
    .copy = copy,
    .create = create,
    .destroy = destroy,
    .equals = equals,
    .fill = fill,
    .first = first,
    .flip = flip,
    .is_empty = is_empty,
    .last = last,
    .length = length,
    .pop = pop,
    .pull = pull,
    .push = push,
    .reduce = reduce,
    .remove = remove,
    .size = size,
    .size_of = size_of,
    .sort = sort,
    .swap = swap,
    .units = units
};
