#ifndef ENCLAVE_T_H__
#define ENCLAVE_T_H__

#include "sgx_edger8r.h" /* for sgx_ocall etc. */
#include <stddef.h>
#include <stdint.h>
#include <wchar.h>

#include <stdlib.h> /* for size_t */

#define SGX_CAST(type, item) ((type)(item))

#ifdef __cplusplus
extern "C" {
#endif

int generateKeyEnclave(uint8_t* sealed_key, size_t sealedkey_len);
int loadKeyEnclave(uint8_t* key, size_t len);
int enclaveProcess(void* inQueue);

sgx_status_t SGX_CDECL ocall_print_string(const char* str);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
