#include "crypto.h"
#include "debug.h"
#include <cstdio>
#include <ctime>
#include <cstdlib>

/* FIXME: remember to replace this with your real key */
const uint8_t hard_coded_enc_key[32] = {
    0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12,
    0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34, 0x34,
    0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12
}; // TA_DERIVED_KEY_MAX_SIZE

// #define DEBUG_OUTPUT

static void randombytes_buf(uint8_t *iv, size_t sz)
{
    srand(time(NULL));
    for (int i = 0; i < sz; i++) {
        iv[i] = rand() % 255;
    }
}

static __thread bool inited = false;
static __thread mbedtls_gcm_context aes;

/* encryption should gurantee that ciphertext locate at the end of out_text.
to allow for cutting the data size short. */
/* IV + TAG + cihpertext */
int gcm_encrypt(uint8_t* in, uint64_t in_sz, uint8_t* out, uint64_t* out_sz)
{
    int res = 0;
    if (!inited) {
        // init the context...
        mbedtls_gcm_init(&aes);
        // Set the key. This next line could have CAMELLIA or ARIA as our GCM mode cipher...
        res = mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, (const unsigned char*)hard_coded_enc_key, sizeof(hard_coded_enc_key) * 8);
        inited = true;
        // printf("setkey %d\n", inited);
    }
    uint8_t* iv_pos = out;
    uint8_t* tag_pos = out + IV_SIZE;
    uint8_t* data_pos = out + IV_SIZE + TAG_SIZE;

    uint8_t iv[IV_SIZE] = { 0 };
    randombytes_buf(iv, sizeof iv); // add entropy
    memcpy(iv_pos, iv, IV_SIZE);

    // Initialise the GCM cipher...
    res = mbedtls_gcm_crypt_and_tag(&aes, MBEDTLS_GCM_ENCRYPT, in_sz, iv_pos, IV_SIZE, NULL, 0, in, data_pos, TAG_SIZE, tag_pos);
    *out_sz = in_sz + IV_SIZE + TAG_SIZE;

#ifdef DEBUG_OUTPUT
    printf("huk-key length %d\n", sizeof(huk_key));
    _print_hex("huk-key: ", huk_key, sizeof(huk_key));
    _print_hex("enc-txt-plain: ", (void*)in, INT32_LENGTH);
    _print_hex("enc-iv: ", (void*)hdr->iv, IV_SIZE);
    _print_hex("enc-tag: ", (void*)hdr->tag, TAG_SIZE);
    _print_hex("enc-txt: ", (void*)hdr->hard_coded_enc_key, INT32_LENGTH);
#endif
    return res;
}

int gcm_decrypt(uint8_t* in, uint64_t in_sz, uint8_t* out, uint64_t* out_sz)
{
    int res = 0;
    if (!inited) {
        // init the context...
        mbedtls_gcm_init(&aes);
        // Set the key. This next line could have CAMELLIA or ARIA as our GCM mode cipher...
        res = mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, (const unsigned char*)hard_coded_enc_key, sizeof(hard_coded_enc_key) * 8);
        inited = true;
    }

    uint8_t* iv_pos = in;
    uint8_t* tag_pos = in + IV_SIZE;
    uint8_t* data_pos = in + IV_SIZE + TAG_SIZE;

    uint32_t data_sz = in_sz - IV_SIZE - TAG_SIZE;
    *out_sz = data_sz;

    res = mbedtls_gcm_auth_decrypt(&aes, data_sz, iv_pos, IV_SIZE, NULL, 0, tag_pos, TAG_SIZE, data_pos, out);
    if (res != 0) {
        printf("decrypt res %d\n", res);
    }
    return res;
}
