#pragma once

#include <enc_types.h>

int enc_float_cmp(EncFloat* left, EncFloat* right, int* res);
int enc_float_add(EncFloat* left, EncFloat* right, EncFloat* res);
int enc_float_sub(EncFloat* left, EncFloat* right, EncFloat* res);
int enc_float_mult(EncFloat* left, EncFloat* right, EncFloat* res);
int enc_float_div(EncFloat* left, EncFloat* right, EncFloat* res);
int enc_float_pow(EncFloat* left, EncFloat* right, EncFloat* res);
int enc_float_mod(EncFloat* left, EncFloat* right, EncFloat* res);
int enc_float_sum_bulk(size_t bulk_size, EncFloat* bulk_data, EncFloat* res);
int enc_float_eval_expr(size_t arg_cnt, Str expr, EncFloat** arg_data, EncFloat* res);

int enc_float_encrypt(float in, EncFloat* out);
int enc_float_decrypt(EncFloat* in, float* out);
