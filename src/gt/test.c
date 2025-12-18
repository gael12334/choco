/*
    Copyright © 2025 Gaël Fortier <gael.fortier.1@ens.etsmtl.ca>
*/

#include "test.h"
#include <stdlib.h>
#include <string.h>

#define COLOR_F(fore, text) "\x1b[" #fore "m" text "\x1b[0m "
#define COLOR_FB(fore, back, text) "\x1b[" #fore ";" #back "m" text "\x1b[0m "

// - - - - - - - - -

void _gt_bin_op(const char* op, const _gt_str_t param1, const _gt_str_t param2, const _gt_str_t expr1, const _gt_str_t expr2, _gt_info_t info)
{
    printf(COLOR_FB(97, 41, "%-50s") "has failed !\n", info.func);
    printf(COLOR_F(91, "Line %u: ") "\n", info.line);
    printf(COLOR_F(91, "%s %s %s") "\n", expr1, op, expr2);
    printf(COLOR_F(91, "%-*s %s %-*s") "\n", (int)strlen(expr1), param1, op, (int)strlen(expr2), param2);
}

// - - - - - - - - -

void _gt_success(const char* func)
{
    printf(COLOR_FB(30, 42, "%-50s") "has passed !\n", func);
}