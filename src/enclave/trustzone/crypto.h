
#ifndef TA_CRYPTO_H
#define TA_CRYPTO_H

#include <assert.h> // for debuggin
#include <mbedtls/gcm.h>
#include <ops_ta.h>
#include <string.h>
#include <string_ext.h>
#include <util.h>

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#define IV_SIZE 12
#define TAG_SIZE 16
#define MAX_BUF_SIZE 512

static uint8_t enc_key[32] = { 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12 }; // TA_DERIVED_KEY_MAX_SIZE
static bool enc_key_loaded = false;

int gcm_encrypt(uint8_t* in, uint32_t in_sz,
    uint8_t* out, uint32_t* out_sz);
int gcm_decrypt(uint8_t* in, uint32_t in_sz,
    uint8_t* out, uint32_t* out_sz);
void _print_hex(const char* what, const void* v, const unsigned long l);

#endif