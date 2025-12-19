/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "../arraylist.h"
#include "defines.h"
#include "header.h"
#include "result.h"
#include <string.h>

/*
 * Private functions
 */

static int memmgr_is_valid(_choco_memmgr_obj memory)
{
    return memory.alloc != NULL && memory.dealloc != NULL;
}

static int arraylist_is_valid(arraylist obj)
{
    return obj.ptr != NULL;
}

static int index_is_valid(header* h, size_t index)
{
    return index < h->length;
}

static int range_is_valid(header* h, size_t index, size_t size)
{
    return index < h->length && (index + size) <= h->length;
}

static int units_is_valid(header* h, size_t units)
{
    return h->units == units;
}

static int alloc_was_successful(void* v, result status)
{
    return v != NULL && status == _CHOCO_ARRAYLIST_OK;
}

static int arraylist_is_full(header* h)
{
    return h->length == h->size;
}

static int size_will_change(size_t current, size_t _new)
{
    return current != _new;
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

static void set_data_from_arraylist(arraylist obj, size_t s_index, arraylist other, size_t o_index, size_t units, size_t size)
{
    void* s_element = get_element(obj, units, s_index);
    void* o_element = get_element(other, units, o_index);
    memcpy(s_element, o_element, get_data_size(units, size));
}

static void set_data_from_pointer(arraylist obj, size_t s_index, void* other, size_t o_index, size_t units, size_t size)
{
    void* s_element = get_element(obj, units, s_index);
    void* o_element = other + get_data_size(units, o_index);
    memcpy(s_element, o_element, get_data_size(units, size));
}

/*
 * Public functions
 */

static arraylist create(memmgr memory, size_t units, size_t size, result* out)
{
    if (!memmgr_is_valid(memory)) {
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
    header* ptr = memory.alloc(memory.obj, needed, (int*)&status);

    if (!alloc_was_successful(ptr, status)) {
        set_result(out, _CHOCO_ARRAYLIST_ERR_ALLOC);
        return get_null_arraylist();
    }

    *ptr = h;
    set_result(out, _CHOCO_ARRAYLIST_OK);
    return get_arraylist(ptr);
}

static arraylist resize(arraylist self, size_t size, result* out)
{
    if (!arraylist_is_valid(self)) {
        set_result(out, _CHOCO_ARRAYLIST_INV_SELF);
        return self;
    }

    header* h = get_header(self);
    if (!size_will_change(h->size, size)) {
        set_result(out, _CHOCO_ARRAYLIST_OK); // premature return and ok if size is identical.
        return self;
    }

    result r;
    arraylist new_self = interface.create(h->memory, h->units, size, &r);
    if (r != _CHOCO_ARRAYLIST_OK) {
        set_result(out, r);
        return self;
    }

    set_data_from_arraylist(new_self, 0, self, 0, h->units, h->size);

    interface.destroy(self, &r);
    if (r != _CHOCO_ARRAYLIST_OK) {
        interface.destroy(new_self, NULL); // the result of this destroy call is irrelevant.
        set_result(out, r);
        return self;
    }

    set_result(out, _CHOCO_ARRAYLIST_OK);
    return new_self;
}

static arraylist clone(memmgr memory, arraylist other, size_t index, size_t size, result* out)
{
    if (!arraylist_is_valid(other)) {
        set_result(out, _CHOCO_ARRAYLIST_INV_OTHER);
        return get_null_arraylist();
    }

    header* other_header = get_header(other);
    if (!range_is_valid(other_header, index, size)) {
        set_result(out, _CHOCO_ARRAYLIST_INV_RANGE);
        return get_null_arraylist();
    }

    result status;
    arraylist self = interface.create(memory, other_header->units, other_header->size, &status);
    if (!alloc_was_successful(self.ptr, status)) {
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
    if (!arraylist_is_valid(self)) {
        set_result(out, _CHOCO_ARRAYLIST_INV_SELF);
        return self;
    }

    header* h = get_header(self);
    if(!units_is_valid(h, units)) {
        set_result(out, _CHOCO_ARRAYLIST_INV_SIZE);
        return self;
    }

    if (arraylist_is_full(h)) {
        arraylist new_self = interface.resize(self, (h->size + 1) * 2, out);
        if(!arraylist_is_valid(new_self)) {
            return self;
        }

        self = new_self;
        h = get_header(self);
    }

    set_data_from_pointer(self, h->length, ref, 0, units, 1);
    h->length++;
    set_result(out, _CHOCO_ARRAYLIST_OK);
    return self;
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
    .resize = resize,
    .size = size,
    .size_of = size_of,
    .sort = sort,
    .swap = swap,
    .units = units
};
