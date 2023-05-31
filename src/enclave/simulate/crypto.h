
#ifndef TA_CRYPTO_H
#define TA_CRYPTO_H

#include <assert.h> // for debuggin
#include <mbedtls/gcm.h>
#include <stdbool.h>
#include <string.h>

#define IV_SIZE 12
#define TAG_SIZE 16
#define MAX_BUF_SIZE 512

static uint8_t enc_key[32] = { 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12 }; // TA_DERIVED_KEY_MAX_SIZE
static bool enc_key_loaded = false;

#ifdef __cplusplus
extern "C" {
#endif

int gcm_encrypt(uint8_t* in, uint64_t in_sz,
    uint8_t* out, uint64_t* out_sz);
int gcm_decrypt(uint8_t* in, uint64_t in_sz,
    uint8_t* out, uint64_t* out_sz);
void _print_hex(const char* what, const void* v, const unsigned long l);

#ifdef __cplusplus
}
#endif

#endif