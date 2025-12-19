/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <stddef.h>
#include "arraylist/defines.h"
#include "arraylist/result.h"
#include "arraylist/header.h"
#include "memmgr.h"

typedef struct arraylist {
   void* ptr;
} arraylist;

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

#include "arraylist/undefs.h"
