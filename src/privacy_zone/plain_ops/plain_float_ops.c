// SPDX-License-Identifier: Mulan PSL v2
/*
 * Copyright (c) 2021 - 2025 The HEDB Project.
 */

#include <defs.h>
#include <plain_float_ops.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(TEE_TZ)
extern double pow(double x, int y);
#else
#include <math.h>
#endif

float plain_float_calc(int reqType, float left, float right)
{
    double res = 0;
    switch (reqType) /* req->common.op */
    {
    case CMD_FLOAT_PLUS:
        res = left + right;
        break;
    case CMD_FLOAT_MINUS:
        res = left - right;
        break;
    case CMD_FLOAT_MULT:
        res = left * right;
        break;
    case CMD_FLOAT_DIV:
        res = left / right;
        break;
    case CMD_FLOAT_EXP:
        res = pow(left, right);
        break;
    case CMD_FLOAT_MOD:
        res = (int)left % (int)right;
        break;
    default:
        break;
    }
    return res;
}

float plain_float_cmp(float left, float right)
{
    return (left == right) ? 0 : (left < right) ? -1 : 1;
}

double plain_float_sum_bulk(int size, float* array)
{
    double res = 0;

    for (int i = 0; i < size; i++) {
        res += array[i];
    }
    return res;
}

/**
 * @brief evaluates and calculates prefix notation expression
 *
 * @param expr
 * @return float
 */
float plain_float_eval_expr(unsigned char* expr, float* arr)
{
    float stack[EXPR_STACK_MAX_SIZE];
    size_t i;
    int stack_top_pos = -1;
    float op1, op2;
    char c;
    memset(stack, 0, (size_t)EXPR_STACK_MAX_SIZE * sizeof(float));
    for (i = 0; i < strlen(expr); ++i) {
        c = expr[i];
        if ((int8_t)c > 0) {
            stack_top_pos++;
            stack[stack_top_pos] = arr[c - 1];
        } else {
            c = (char)-c;
            if (c == '#') {
                if (stack_top_pos < 0) {
                    printf("Missing operand!\n");
                    // exit(0);
                }
                stack[stack_top_pos] = -stack[stack_top_pos];
            } else {
                if (stack_top_pos < 1) {
                    printf("Missing operand!\n");
                    // exit(0);
                }
                op1 = (float)stack[stack_top_pos--];
                op2 = (float)stack[stack_top_pos];
                switch (c) {
                case '+':
                    stack[stack_top_pos] = op2 + op1;
                    break;
                case '-':
                    stack[stack_top_pos] = op2 - op1;
                    break;
                case '*':
                    stack[stack_top_pos] = op2 * op1;
                    break;
                case '/':
                    stack[stack_top_pos] = op2 / op1;
                    break;
                case '%':
                    stack[stack_top_pos] = (float)((int)op2 % (int)op1);
                    break;
                case '^':
                    stack[stack_top_pos] = (float)pow((double)op2, (double)op1);
                    break;
                default:
                    printf("No matching operand!\n");
                    // exit(0);
                }
                if (c == '+') {
                    stack[stack_top_pos] = op1 + op2;
                }
            }
        }
    }
    return stack[stack_top_pos];
}
