/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#pragma once
#include <stdio.h>
#include <string.h>

typedef const char *_gt_str_t;

typedef struct _gt_info
{
    const char *file;
    const char *func;
    unsigned line;
} _gt_info_t;

void _gt_bin_op(const char *op, const _gt_str_t param1, const _gt_str_t param2, const _gt_str_t expr1, const _gt_str_t expr2, _gt_info_t info);

void _gt_success(const char *func);

#define _gt_passed() _gt_success(__func__);

#define _gt_test(fn, opt_case) static void test_##fn##_##opt_case(void)

#define _gt_run(fn, opt_case) test_##fn##_##opt_case()

#define _gt_get_info() \
    (_gt_info_t) { .file = __FILE__, .func = __func__, .line = __LINE__ }

#define _gt_test_int_bin_op(n1, op, n2)                            \
    {                                                              \
        long _n1 = n1, _n2 = n2;                                   \
        if (!(n1 op n2))                                           \
        {                                                          \
            char _nb1[20], _nb2[20];                               \
            sprintf(_nb1, "%li", _n1);                             \
            sprintf(_nb2, "%li", _n2);                             \
            _gt_bin_op(#op, _nb1, _nb2, #n1, #n2, _gt_get_info()); \
            return;                                                \
        }                                                          \
    }

#define _gt_test_float_bin_op(n1, op, n2)                          \
    {                                                              \
        double _n1 = n1, _n2 = n2;                                 \
        if (!(_n1 op _n2))                                         \
        {                                                          \
            char _nb1[50], _nb2[50];                               \
            sprintf(_nb1, "%lf", _n1);                             \
            sprintf(_nb2, "%lf", _n2);                             \
            _gt_bin_op(#op, _nb1, _nb2, #n1, #n2, _gt_get_info()); \
            return;                                                \
        }                                                          \
    }

#define _gt_test_str_bin_op(n1, op, n2)                            \
    {                                                              \
        const char *_n1 = n1, _n2 = n2;                            \
        if (!(strcmp(_n1, _n2) op 0))                              \
        {                                                          \
            _gt_bin_op(#op, _nb1, _nb2, #n1, #n2, _gt_get_info()); \
            return;                                                \
        }                                                          \
    }

#define _gt_test_print_line(len)  \
    for (int i = 0; i < len; i++) \
    {                             \
        putchar('=');             \
    }                             \
    putchar('\n');

#define _gt_test_print(format, ...)                     \
    {                                                   \
        _gt_test_print_line(50);                        \
        printf("'%s', Line %u:\n", __func__, __LINE__); \
        _gt_test_print_line(50);                        \
        printf(format, __VA_ARGS__);                    \
        _gt_test_print_line(50);                        \
    }

#define _gt_test_int_eq(n1, n2) \
    _gt_test_int_bin_op(n1, ==, n2)

#define _gt_test_ptr_eq(n1, n2) \
    _gt_test_int_eq((long)n1, (long)n2)

#define _gt_test_float_eq(n1, n2) \
    _gt_test_float_bin_op(n1, ==, n2)

#define _gt_test_str_eq(n1, n2) \
    _gt_test_str_bin_op(n1, ==, n2)

// -

#define _gt_test_int_neq(n1, n2) \
    _gt_test_int_bin_op(n1, !=, n2)

#define _gt_test_ptr_neq(n1, n2) \
    _gt_test_int_neq((long)n1, (long)n2)

#define _gt_test_float_neq(n1, n2) \
    _gt_test_float_bin_op(n1, !=, n2)

#define _gt_test_str_neq(n1, n2) \
    _gt_test_str_bin_op(n1, !=, n2)

// -

#define _gt_test_int_gt(n1, n2) \
    _gt_test_int_bin_op(n1, >, n2)

#define _gt_test_float_gt(n1, n2) \
    _gt_test_float_bin_op(n1, >, n2)

// -

#define _gt_test_int_lt(n1, n2) \
    _gt_test_int_bin_op(n1, <, n2)

#define _gt_test_float_lt(n1, n2) \
    _gt_test_float_bin_op(n1, <, n2)

// -

#define _gt_test_int_gte(n1, n2) \
    _gt_test_int_bin_op(n1, >=, n2)

#define _gt_test_float_gte(n1, n2) \
    _gt_test_float_bin_op(n1, >=, n2)

// -

#define _gt_test_int_lte(n1, n2) \
    _gt_test_int_bin_op(n1, <=, n2)

#define _gt_test_float_lte(n1, n2) \
    _gt_test_float_bin_op(n1, <=, n2)
