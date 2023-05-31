#include "crypto.h"

// #define DEBUG_OUTPUT

int isgraph(int c) { return (unsigned)c - 0x21 < 0x5e; }

void _print_hex(const char* what, const void* v, const unsigned long l)
{
    const unsigned char* p = v;
    unsigned long x, y = 0, z;
    printf("%s contents: \n", what);
    for (x = 0; x < l;) {
        printf("%02x ", p[x]);
        if (!(++x % 16) || x == l) {
            if ((x % 16) != 0) {
                z = 16 - (x % 16);
                if (z >= 8)
                    printf(" ");
                for (; z != 0; --z) {
                    printf("   ");
                }
            }
            printf(" | ");
            for (; y < x; y++) {
                if ((y % 8) == 0)
                    printf(" ");
                if (isgraph(p[y]))
                    printf("%c", p[y]);
                else
                    printf(".");
            }
            printf("\n");
        } else if ((x % 8) == 0) {
            printf(" ");
        }
    }
}
static bool inited = false;

static mbedtls_gcm_context aes;

/* encryption should gurantee that ciphertext locate at the end of out_text.
to allow for cutting the data size short. */
/* IV + TAG + cihpertext */
int gcm_encrypt(uint8_t* in,
    uint32_t in_sz, uint8_t* out, uint32_t* out_sz)
{
    int res = 0;
    if (!inited) {
        // init the context...
        mbedtls_gcm_init(&aes);
        // printf("inited. %d\n", inited);
        // printf("location of mbedtls_gcm_setkey %x.\n", mbedtls_gcm_setkey );
        // Set the key. This next line could have CAMELLIA or ARIA as our GCM mode cipher...
        res = mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, (const unsigned char*)enc_key, sizeof(enc_key) * 8);
        inited = true;
        // printf("setkey %d\n", inited);
    }
    uint8_t* iv_pos = out;
    uint8_t* tag_pos = out + IV_SIZE;
    uint8_t* data_pos = out + IV_SIZE + TAG_SIZE;

    uint8_t iv[IV_SIZE] = { 0 };
    memcpy(iv_pos, iv, IV_SIZE);

    // Initialise the GCM cipher...
    res = mbedtls_gcm_crypt_and_tag(&aes, MBEDTLS_GCM_ENCRYPT, in_sz, iv_pos, IV_SIZE, NULL, 0, in, data_pos, TAG_SIZE, tag_pos);
    // char buffer[1000];
    // printf("before start\n");
    // res = mbedtls_gcm_starts(&aes, MBEDTLS_GCM_ENCRYPT, iv, IV_SIZE, NULL, 0 );
    // if(res != 0) {
    // 	printf("error start %d\n",res);
    // }
    // printf("start\n");
    // res = mbedtls_gcm_update(&aes, in_sz, in, out);
    // if(res != 0) {
    // 	printf("error update %d\n",res);
    // }
    // printf("update\n");
    // res = mbedtls_gcm_finish(&aes, out + IV_SIZE + in_sz, TAG_SIZE);
    // if(res != 0) {
    // 	printf("error finish %d\n",res);
    // }
    // printf("finish\n");
    // memcpy(out + in_sz, iv, IV_SIZE); // copy iv to out.
    *out_sz = in_sz + IV_SIZE + TAG_SIZE;
    // randombytes_buf(nonce, sizeof nonce); TODO: ADD a way to get entropy

    // _print_hex("enc-txt-plain: ", (void *)in, in_sz);
    // _print_hex("enc-iv: ", (void *)iv_pos, IV_SIZE);
    // _print_hex("enc-tag: ", (void *)tag_pos, TAG_SIZE);
    // _print_hex("enc-txt: ", (void *)data_pos, in_sz);
#ifdef DEBUG_OUTPUT
    printf("huk-key length %d\n", sizeof(huk_key));
    _print_hex("huk-key: ", huk_key, sizeof(huk_key));
    _print_hex("enc-txt-plain: ", (void*)in, INT32_LENGTH);
    _print_hex("enc-iv: ", (void*)hdr->iv, IV_SIZE);
    _print_hex("enc-tag: ", (void*)hdr->tag, TAG_SIZE);
    _print_hex("enc-txt: ", (void*)hdr->enc_key, INT32_LENGTH);
#endif
    return res;
}

int gcm_decrypt(uint8_t* in,
    uint32_t in_sz, uint8_t* out, uint32_t* out_sz)
{
    if (!inited) {
        // init the context...
        mbedtls_gcm_init(&aes);
        // Set the key. This next line could have CAMELLIA or ARIA as our GCM mode cipher...
        mbedtls_gcm_setkey(&aes, MBEDTLS_CIPHER_ID_AES, (const unsigned char*)enc_key, strlen(enc_key) * 8);
        inited = true;
    }

    uint8_t* iv_pos = in;
    uint8_t* tag_pos = in + IV_SIZE;
    uint8_t* data_pos = in + IV_SIZE + TAG_SIZE;

    int res = 0;
    uint32_t data_sz = in_sz - IV_SIZE - TAG_SIZE;
    *out_sz = data_sz;

    res = mbedtls_gcm_auth_decrypt(&aes, data_sz, iv_pos, IV_SIZE, NULL, 0, tag_pos, TAG_SIZE, data_pos, out);

    return res;
}
