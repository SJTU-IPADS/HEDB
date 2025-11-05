#pragma once

#ifdef __cplusplus
extern "C" {
#endif

float plain_float_calc(int reqType, float left, float right);
float plain_float_cmp(float left, float right);
double plain_float_sum_bulk(int size, float* array);
float plain_float_eval_expr(unsigned char* expr, float* arr);

#ifdef __cplusplus
}
#endif
