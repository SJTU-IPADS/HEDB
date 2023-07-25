#pragma once
#include <defs.h>
#include <math.h>
#include <request_types.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DECRYPT_THREAD 10

int encrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len);
int decrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len);
int decrypt_bytes_para(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len);

void decrypt_wait(uint8_t* pDst, size_t exp_dst_len);

#ifdef __cplusplus
}
#endif
