#include "sgx/enclave.hpp"
#include "sgx/enclave_t.h"

#include <request_types.h>
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <sync.h>

void printf(const char* fmt, ...)
{
    char buf[BUFSIZ] = { '\0' };
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZ, fmt, ap);
    va_end(ap);
    ocall_print_string(buf);
}
// void _print_hex(const char *what, const void *v, const unsigned long l)
// {
//         const unsigned char *p = v;
//         unsigned long x, y = 0, z;
//         printf("%s contents: \n", what);
//         for (x = 0; x < l;)
//         {
//                 printf("%02x ", p[x]);
//                 if (!(++x % 16) || x == l)
//                 {
//                         if ((x % 16) != 0)
//                         {
//                                 z = 16 - (x % 16);
//                                 if (z >= 8)
//                                         printf(" ");
//                                 for (; z != 0; --z)
//                                 {
//                                         printf("   ");
//                                 }
//                         }
//                         printf(" | ");
//                         for (; y < x; y++)
//                         {
//                                 if ((y % 8) == 0)
//                                         printf(" ");
//                                 if (isgraph(p[y]))
//                                         printf("%c", p[y]);
//                                 else
//                                         printf(".");
//                         }
//                         printf("\n");
//                 }
//                 else if ((x % 8) == 0)
//                 {
//                         printf(" ");
//                 }
//         }
// }
sgx_aes_ctr_128bit_key_t* p_key = NULL;

void free_allocated_memory(void* pointer)
{
    if (pointer != NULL) {
        free(pointer);
        pointer = NULL;
    }
}

/* Generate a master key
 @input: uint8_t sealed_key - pointer to sealed master key array
         size_t - length of the array (=
 sgx_calc_sealed_data_size(sgx_aes_ctr_128bit_key_t) = 576)
 @return:
    * SGX_error, if there was an error during seal function
    0, otherwise
*/
int generateKeyEnclave(uint8_t* sealed_key, size_t sealedkey_len)
{
    int resp = SGX_SUCCESS;
    uint32_t len = sizeof(sgx_aes_ctr_128bit_key_t);
    uint8_t* p_key_tmp = (uint8_t*)malloc(len);

    if (sgx_calc_sealed_data_size(0, len) > sealedkey_len)
        return MEMORY_COPY_ERROR;

    sgx_read_rand(p_key_tmp, len);
    resp = sgx_seal_data(
        0, NULL, len, p_key_tmp, sealedkey_len, (sgx_sealed_data_t*)sealed_key);

    memset_s(p_key_tmp, len, 0, len);
    free_allocated_memory(p_key_tmp);

    return resp;
}

/* Load the master key from sealed data
 *  @input: uint8_t sealed_key - pointer to a sealed data byte array
            size_t - length of the array (=
 sgx_calc_sealed_data_size(sgx_aes_ctr_128bit_key_t) = 576)
 @return:
    * SGX_error, if there was an error during unsealing
    0, otherwise
*/
int loadKeyEnclave(uint8_t* sealed_key, size_t sealedkey_len)
{
    int resp = SGX_SUCCESS;
    uint32_t len = sizeof(sgx_aes_ctr_128bit_key_t);

    if (p_key == NULL)
        p_key = new sgx_aes_ctr_128bit_key_t[len];

    if (sgx_calc_sealed_data_size(0, sizeof(sgx_aes_ctr_128bit_key_t)) > sealedkey_len)
        return MEMORY_COPY_ERROR;

    resp = sgx_unseal_data(
        (const sgx_sealed_data_t*)sealed_key, NULL, NULL, (uint8_t*)p_key, &len);

    return resp;
}

/* Decrypts byte array by aesgcm mode
 @input: uint8_t array - pointer to encrypted byte array
         size_t - length of encrypted  array
         uint8_t array - pointer to decrypted array
         size_t - length of decrypted array (length of array -
 SGX_AESGCM_IV_SIZE - SGX_AESGCM_MAC_SIZE)
 @return:
    * SGX_error, if there was an error during encryption/decryption
    0, otherwise
*/
int decrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t dst_len)
{
    uint8_t* iv_pos = pSrc;
    uint8_t* tag_pos = pSrc + IV_SIZE;
    uint8_t* data_pos = pSrc + IV_SIZE + TAG_SIZE;

    int resp = sgx_rijndael128GCM_decrypt(
        p_key,
        data_pos, // cipher
        dst_len,
        pDst, // plain out
        iv_pos,
        SGX_AESGCM_IV_SIZE, // nonce
        NULL,
        0, // aad
        (sgx_aes_gcm_128bit_tag_t*)(tag_pos)); // tag

    return resp;
}

/* Encrypts byte array by aesgcm mode
 @input: uint8_t array - pointer to a byte array
         size_t - length of the array
         uint8_t array - pointer to result array
         size_t - length of result array (SGX_AESGCM_IV_SIZE + length of array +
 SGX_AESGCM_MAC_SIZE) inQueue->dequeue();
 @return:
    * SGX_error, if there was an error during encryption/decryption
    0, otherwise
*/
int encrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t dst_len)
{
    unsigned char* nonce = new unsigned char[SGX_AESGCM_IV_SIZE];

    /* set iv to same value. */
    int resp;
    memset(nonce, 0, SGX_AESGCM_IV_SIZE);
    // int resp = sgx_read_rand(nonce, SGX_AESGCM_IV_SIZE);
    // if (resp != SGX_SUCCESS)
    //     return resp;

    /* ope alternative */
    memset(nonce, 0, SGX_AESGCM_IV_SIZE);
    /* end ope alternative */

    uint8_t* iv_pos = pDst;
    uint8_t* tag_pos = pDst + IV_SIZE;
    uint8_t* data_pos = pDst + IV_SIZE + TAG_SIZE;

    memcpy(iv_pos, nonce, SGX_AESGCM_IV_SIZE);
    resp = sgx_rijndael128GCM_encrypt(
        p_key,
        pSrc,
        src_len,
        data_pos,
        nonce,
        SGX_AESGCM_IV_SIZE,
        NULL,
        0,
        (sgx_aes_gcm_128bit_tag_t*)(tag_pos));

    /* ope */
    // if(src_len == INT32_LENGTH || src_len == FLOAT4_LENGTH){
    //     memcpy(pDst + ENC_INT32_GCD_LENGTH, pSrc, INT32_LENGTH);
    // }else {
    //     memcpy(pDst + src_len + SGX_AESGCM_IV_SIZE + SGX_AESGCM_MAC_SIZE, pSrc, src_len);
    // }
    /* end ope */
    delete[] nonce;

    return resp;
}

uint64_t current_cycles()
{
    uint32_t low, high;
    asm volatile("rdtsc"
                 : "=a"(low), "=d"(high));
    return ((uint64_t)low) | ((uint64_t)high << 32);
}

int enclaveProcess(void* arg1)
{
    // printf("enclave process called %p\n", arg1);
    if (arg1 == NULL)
        return -1;
    BaseRequest* req = (BaseRequest*)arg1;
    uint64_t start, end;
    uint64_t total_time = 0;
    while (true) {
        if (req->status != SENT)
            YIELD_PROCESSOR;
        else {
            start = current_cycles();
            // printf("ops called %d", req->reqType);
            handle_ops(req);
            // printf("req returned %d\n",req->reqType);
            req->status = DONE;
            end = current_cycles();
            total_time += end - start;
            printf("time: %d\n", total_time);
        }
    }

    return 0;
}
