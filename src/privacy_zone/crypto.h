#ifndef TA_CRYPTO_H
#define TA_CRYPTO_H

#include <assert.h> // for debugging
#include <mbedtls/gcm.h>
#include <stdbool.h>
#include <string.h>

#define IV_SIZE 12
#define TAG_SIZE 16
#define MAX_BUF_SIZE 512

#ifdef __cplusplus
extern "C" {
#endif

int gcm_encrypt(uint8_t* in, uint64_t in_sz, uint8_t* out, uint64_t* out_sz);
int gcm_decrypt(uint8_t* in, uint64_t in_sz, uint8_t* out, uint64_t* out_sz);

#ifdef __cplusplus
}
#endif

#endif
