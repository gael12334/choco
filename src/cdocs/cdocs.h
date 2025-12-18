/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <stddef.h>

typedef struct _cdocs_fn_r_struct {
    const char* returns;
    const char* description;
} _cdocs_fn_r_struct;

#define _cdocs_ending_p (_cdocs_fn_p_struct){.signature = NULL, .description = NULL}
typedef struct _cdocs_fn_p_struct {
    const char* signature;
    const char* description;
} _cdocs_fn_p_struct;

typedef struct _cdocs_fn_f_struct {
    const char* name;
    const char* description;
} _cdocs_fn_f_struct;

#define _cdocs_fn_p_count (20)
typedef struct _cdocs_fn_struct {
    _cdocs_fn_f_struct function;
    _cdocs_fn_r_struct returns;
    _cdocs_fn_p_struct parameters[_cdocs_fn_p_count + 1]; // +1: array must end with _cdocs_ending_p.
} _cdocs_fn_struct;

void _cdocs_generate_documentation_x(const char* filename, const char* subject, size_t size, _cdocs_fn_struct* functions);

#define _cdocs_fn_p(_sig, _desc) (_cdocs_fn_p_struct) {.signature = #_sig, .description = _desc}

#define _cdocs_fn_r(_type, _desc) (_cdocs_fn_r_struct) {.returns = #_type, .description = _desc}

#define _cdocs_fn_f(_name, _desc) (_cdocs_fn_f_struct) {.name = #_name, .description = _desc}

#define _cdocs_fn(f, r, ...) (_cdocs_fn_struct){.function = f, .returns = r, .parameters = {__VA_ARGS__, _cdocs_ending_p}}

#define _cdocs_code(x) "`" #x "`"

#define _cdocs_generate_documentation(subject, functions) \
    _cdocs_generate_documentation_x(__FILE__ ".md", subject, sizeof(functions) / sizeof(functions[0]), functions)
