#pragma once

float plain_float_calc(int reqType, float left, float right);
float plain_float_cmp(float left, float right);
double plain_float_bulk(int reqType, int size, float* array);
float plain_float_eval_expr(char* expr, float* arr);