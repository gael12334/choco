/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <stddef.h>

#define arraylist _choco_arraylist_obj
#define result _choco_arraylist_result
#define memmgr _choco_arraylist_memmgr
#define interface _choco_arraylist
#define header_size _choco_arraylist_header_size

enum result {
    _CHOCO_ARRAYLIST_OK,
    _CHOCO_ARRAYLIST_INV_MEMMGR,
    _CHOCO_ARRAYLIST_INV_UNITS,
    _CHOCO_ARRAYLIST_INV_INDEX,
    _CHOCO_ARRAYLIST_INV_REF,
    _CHOCO_ARRAYLIST_INV_SELF,
    _CHOCO_ARRAYLIST_INV_SIZE,
    _CHOCO_ARRAYLIST_INV_RANGE,
    _CHOCO_ARRAYLIST_INV_OTHER,
    _CHOCO_ARRAYLIST_INV_OFFSET,
    _CHOCO_ARRAYLIST_ERR_ALLOC,
    _CHOCO_ARRAYLIST_ERR_DEALLOC,
    _CHOCO_ARRAYLIST_ERR_EMPTY,
    _CHOCO_ARRAYLIST_NOT_IMPL,
};

struct memmgr {
    void* obj;
    void*(*alloc)(void* obj, size_t size, enum result* out);
    void(*dealloc)(void* obj, void* ptr, enum result* out);
};

struct arraylist {
   void* ptr;
};

typedef struct memmgr memmgr;
typedef struct arraylist arraylist;
typedef enum result result;

extern const size_t header_size;

extern struct interface {
    arraylist(*create)(memmgr memory, size_t units, size_t size, result* out);
    arraylist(*resize)(arraylist self, size_t size, result* out);
    arraylist(*clone)(memmgr memory, arraylist other, size_t index, size_t size, result* out);
    arraylist(*push)(arraylist self, void* ref, size_t units, result* out);
    arraylist(*pull)(arraylist self, arraylist other, size_t index, size_t size, result* out);
    arraylist(*reduce)(arraylist self, result* out);
    size_t(*size_of)(arraylist self, result* out);
    size_t(*length)(arraylist self, result* out);
    size_t(*units)(arraylist self, result* out);
    size_t(*size)(arraylist self, result* out);
    void*(*remove)(arraylist self, void* ref, size_t units, result* out);
    void*(*first)(arraylist self, result* out);
    void*(*last)(arraylist self, result* out);
    void*(*at)(arraylist self, size_t index, result* out);
    void(*destroy)(arraylist self, result* out);
    void(*fill)(arraylist self, void* ref, size_t units, size_t index, size_t size, result* out);
    void(*copy)(arraylist self, arraylist other, size_t index, size_t size, result* out);
    void(*clear)(arraylist self, result* out);
    void(*swap)(arraylist self, size_t index, size_t other, result* out);
    void(*flip)(arraylist self, result* out);
    void(*sort)(arraylist self, size_t units, size_t offset, result* out);
    int(*is_empty)(arraylist self, result* out);
    int(*equals)(arraylist self, arraylist other, result* out);
    int(*pop)(arraylist self, size_t size, result* out);
} interface;


#undef arraylist
#undef result
#undef memmgr
#undef interface
#undef header_size
