// SPDX-License-Identifier: Mulan PSL v2
/*
 * Copyright (c) 2021 - 2023 The HEDB Project.
 */

#include "plain_int_ops.h"
#include "plain_text_ops.h"
#include "plain_timestamp_ops.h"
#include "prefix_udf.h"
#include <defs.h>
// SPDX-License-Identifier: Mulan PSL v2
/*
 * Copyright (c) 2021 - 2023 The HEDB Project.
 */

#include <klee/klee.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PREFIX_SIZE 100
int prefix_overlap_wrapper()
{
    prefix_range *a = (prefix_range*)malloc(MAX_PREFIX_SIZE),
                 *b = (prefix_range*)malloc(MAX_PREFIX_SIZE);
    klee_make_symbolic(a, MAX_PREFIX_SIZE, "prefix_overlap_a");
    klee_make_symbolic(b, MAX_PREFIX_SIZE, "prefix_overlap_b");
    bool is_overlap = pr_overlaps(a, b);
    free(a);
    free(b);
    return 0;
}

#define MAX_TEXT_LENGTH 100
int text_cmp_wrapper()
{
    char left[MAX_TEXT_LENGTH];
    char right[MAX_TEXT_LENGTH];
    klee_make_symbolic(left, MAX_TEXT_LENGTH, "text_cmp_left");
    klee_make_symbolic(right, MAX_TEXT_LENGTH, "text_cmp_right");
    int cmp = plain_text_cmp(left, strlen(left), right, strlen(right));
    return 0;
}

int text_like_wrapper()
{
    char left[MAX_TEXT_LENGTH];
    char right[MAX_TEXT_LENGTH];
    klee_make_symbolic(left, MAX_TEXT_LENGTH, "text_cmp_left");
    klee_make_symbolic(right, MAX_TEXT_LENGTH, "text_cmp_right");
    int cmp = plain_text_like(left, strlen(left), right, strlen(right));
    return 0;
}

int text_concat_wrapper()
{
    char a[MAX_TEXT_LENGTH];
    char b[MAX_TEXT_LENGTH];
    klee_make_symbolic(a, MAX_TEXT_LENGTH, "text_concat_left");
    klee_make_symbolic(b, MAX_TEXT_LENGTH, "text_concat_right");
    int len = strlen(a);
    plain_text_concat(a, (uint32_t*)&len, b, strlen(b));
    return 0;
}

int text_substring_wrapper()
{
    char str[MAX_TEXT_LENGTH];
    char res[MAX_TEXT_LENGTH];
    int start, length;
    klee_make_symbolic(str, MAX_TEXT_LENGTH, "text_substring_str");
    klee_make_symbolic(&start, 4, "text_substring_start");
    klee_make_symbolic(&length, 4, "text_substring_length");
    int resp = plain_text_substring(str, length, start, res);
    return 0;
}

int int_calc_wrapper()
{
    int reqType, left, right;
    klee_make_symbolic(&reqType, 4, "int_calc_type");
    klee_make_symbolic(&left, 4, "int_calc_left");
    klee_make_symbolic(&right, 4, "int_calc_right");
    plain_int32_calc(reqType, left, right);
    return 0;
}

int int_cmp_wrapper()
{
    int left, right;
    klee_make_symbolic(&left, 4, "int_cmp_left");
    klee_make_symbolic(&right, 4, "int_cmp_right");
    plain_int32_cmp(left, right);
    return 0;
}

int int_bulk_wrapper()
{
    int reqType, size;
    int array[BULK_SIZE];
    klee_make_symbolic(&reqType, 4, "int_bulk_type");
    klee_make_symbolic(&size, 4, "int_bulk_size");
    klee_make_symbolic(&array, sizeof(int) * BULK_SIZE, "int_bulk_array");
    plain_int32_bulk(reqType, size, array);
    free(array);
    return 0;
}

int timestamp_cmp_wrapper()
{
    TIMESTAMP left, right;
    klee_make_symbolic(&left, 8, "timestamp_cmp_left");
    klee_make_symbolic(&right, 8, "timestamp_cmp_right");
    plain_timestamp_cmp(left, right);
    return 0;
}

int timestamp_extract_year_wrapper()
{
    TIMESTAMP t;
    klee_make_symbolic(&t, 8, "timestamp_extract_year_t");
    plain_timestamp_extract_year(t);
    return 0;
}

int ops_wrapper()
{
    int op;
    klee_make_symbolic(&op, 4, "op");
    switch (op) {
    case CMD_INT_PLUS:
    case CMD_INT_MINUS:
    case CMD_INT_MULT:
    case CMD_INT_DIV:
    case CMD_INT_EXP:
    case CMD_INT_MOD:
        int_calc_wrapper();
        break;
    case CMD_INT_CMP:
        int_cmp_wrapper();
        break;
    case CMD_INT_SUM_BULK:
        int_bulk_wrapper();
        break;

    /* FLOAT are not supported currrently */
    case CMD_FLOAT_PLUS:
    case CMD_FLOAT_MINUS:
    case CMD_FLOAT_MULT:
    case CMD_FLOAT_DIV:
    case CMD_FLOAT_EXP:
    case CMD_FLOAT_MOD:
    case CMD_FLOAT_CMP:
    case CMD_FLOAT_SUM_BULK:
    case CMD_FLOAT_EVAL_EXPR:
        break;

    /* timestamp*/
    case CMD_TIMESTAMP_EXTRACT_YEAR:
        timestamp_extract_year_wrapper();
        break;
    case CMD_TIMESTAMP_CMP:
        timestamp_cmp_wrapper();
        break;

    /* text */
    case CMD_STRING_SUBSTRING:
        text_substring_wrapper();
        break;
    case CMD_STRING_CONCAT:
        text_concat_wrapper();
        break;
    case CMD_STRING_LIKE:
        text_like_wrapper();
        break;
    case CMD_STRING_CMP:
        text_cmp_wrapper();
        break;

    case CMD_PREFIX_OVERLAP:
        prefix_overlap_wrapper();
        break;
    default:
        break;
    }
    return 0;
}
