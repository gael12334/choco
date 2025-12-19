/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <stddef.h>

typedef struct _choco_memmgr_obj {
    void* obj;
    void*(*alloc)(void* obj, size_t size, int* out);
    void(*dealloc)(void* obj, void* ptr, int* out);
} _choco_memmgr_obj;

