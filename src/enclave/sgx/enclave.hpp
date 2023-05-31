#pragma once

#include "sgx_eid.h"
#include "sgx_tcrypto.h"
#include "sgx_trts.h"
#include "sgx_tseal.h"
#include <assert.h>
#include <stdlib.h>

#include "math.h"
#include <stdio.h>
#include <string.h>

#include <include/enc_ops.h>
#include <enc_types.h>
#include <request_types.h>
#include <defs.h>
#include <sync.h>

#if defined(__cplusplus)
extern "C"
{
#endif

    void printf(const char *fmt, ...);
    
    void free_allocated_memory(void* pointer);

    // FUNCTIONS
    int decrypt_bytes(uint8_t* pSrc, size_t srcLen, uint8_t* pDst, size_t dstLen);
    int encrypt_bytes(uint8_t* pSrc, size_t srcLen, uint8_t* pDst, size_t dstLen);


#if defined(__cplusplus)
}
#endif
