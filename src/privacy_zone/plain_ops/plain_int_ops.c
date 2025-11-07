// SPDX-License-Identifier: Mulan PSL v2
/*
 * Copyright (c) 2021 - 2025 The HEDB Project.
 */

#include <defs.h>
#include <plain_int_ops.h>

#if defined(TEE_TZ)
double pow(double x, int y)
{
    double temp;
    if (y == 0)
        return 1;
    temp = pow(x, y / 2);
    if ((y % 2) == 0) {
        return temp * temp;
    } else {
        if (y > 0)
            return x * temp * temp;
        else
            return (temp * temp) / x;
    }
}
#else
#include <math.h>
#endif

int plain_int32_calc(int reqType, int left, int right)
{
    int res = 0;
    switch (reqType) /* req->common.op */
    {
    case CMD_INT_PLUS:
        res = left + right;
        break;
    case CMD_INT_MINUS:
        res = left - right;
        break;
    case CMD_INT_MULT:
        res = left * right;
        break;
    case CMD_INT_DIV:
        res = left / right;
        break;
    case CMD_INT_POW:
        res = pow(left, right);
        break;
    case CMD_INT_MOD:
        res = (int)left % (int)right;
        break;
    default:
        break;
    }
    return res;
}

int plain_int32_cmp(int left, int right)
{
    return (left == right) ? 0 : (left < right) ? -1 : 1;
}

int plain_int32_sum_bulk(int size, int* array)
{
    int res = 0;

    for (int i = 0; i < size; i++) {
        res += array[i];
    }
    return res;
}
