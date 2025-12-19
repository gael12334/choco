/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include "../memmgr.h"

typedef struct _choco_arraylist_header {
    _choco_memmgr_obj memory;
    size_t units;
    size_t length;
    size_t size;
} _choco_arraylist_header;
