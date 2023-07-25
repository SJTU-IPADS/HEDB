#pragma once

#include <enc_types.h>

int enc_int_add(EncInt* left, EncInt* right, EncInt* res);
int enc_int_sub(EncInt* left, EncInt* right, EncInt* res);
int enc_int_mult(EncInt* left, EncInt* right, EncInt* res);
int enc_int_div(EncInt* left, EncInt* right, EncInt* res);
int enc_int_pow(EncInt* left, EncInt* right, EncInt* res);
int enc_int_mod(EncInt* left, EncInt* right, EncInt* res);
int enc_int_cmp(EncInt* left, EncInt* right, int* res);
int enc_int_encrypt(int pSrc, EncInt* pDst);
int enc_int_decrypt(EncInt* pSrc, int* pDst);
int enc_int_sum_bulk(size_t bulk_size, EncInt* arg1, EncInt* res);
