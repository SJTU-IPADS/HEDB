/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "crypto.h"
#include <arm64_user_sysreg.h>
#include <assert.h> // for debugging
#include <enc_ops.h>
#include <ops_ta.h>
#include <string.h>
#include <string_ext.h>
#include <sync.h>
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <util.h>

#define LOAD_BARRIER asm volatile("dsb ld" :: \
                                      : "memory")
#define STORE_BARRIER asm volatile("dsb st" :: \
                                       : "memory")

// #define __TA_PROFILE
/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
    DMSG("ops TA_CREATE has been called");

    return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
    DMSG("ops TA_DESTROY has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
    TEE_Param __maybe_unused params[4],
    void __maybe_unused** sess_ctx)
{
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE);

    DMSG("has been called");

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    /* Unused parameters */
    (void)&params;
    (void)&sess_ctx;

    /*
     * The DMSG() macro is non-standard, TEE Internal API doesn't
     * specify any means to logging from a TA.
     */
    IMSG("Hello World!\n");

    /* If return value != TEE_SUCCESS the session will not be created. */
    return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused* sess_ctx)
{
    (void)&sess_ctx; /* Unused parameter */
    IMSG("ops Goodbye!\n");
}

static TEE_Result inc_value(uint32_t param_types,
    TEE_Param params[4])
{
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    static int counter = 0;
    counter++;
    if (counter % 100000 == 0) {
        DMSG("%d\n", counter);
    }

    // uint64_t duration =0,timer = 0;
    // uint64_t start,end;
    // uint32_t freq = read_cntfrq();

    // duration = (end - start) * 1000000 / freq;
    // timer += duration;
    // DMSG("duration in us %d",duration);

    params[0].value.a++;
    return TEE_SUCCESS;
}

static TEE_Result gcm_demo(uint32_t param_types,
    TEE_Param params[4])
{
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INOUT,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE);
    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    TEE_Result res = TEE_SUCCESS;

    uint32_t in_sz = 0;
    uint32_t out_sz = 0;
    DMSG("gcm demo called \n");
    char buffer[1000];
    int a = 0x1234;
    memcpy(buffer, &a, INT32_LENGTH);

    in_sz = INT32_LENGTH;
    res = gcm_encrypt((uint8_t*)buffer, in_sz, (uint8_t*)(buffer + INT32_LENGTH), &out_sz);
    DMSG("in %d,struct %d,out %d", in_sz, *(int*)buffer, out_sz);
    if (res == TEE_SUCCESS) {
        assert(out_sz == ENC_INT32_LENGTH);
    }

    memset(buffer, 0, INT32_LENGTH);
    in_sz = ENC_INT32_LENGTH;
    out_sz = INT32_LENGTH;
    res = gcm_decrypt((uint8_t*)(buffer + INT32_LENGTH), in_sz, (uint8_t*)buffer, &out_sz);
    DMSG("decrypt size %d, result %d\n", out_sz, *((int*)buffer));
    return TEE_SUCCESS;
}
static TEE_Result dec_value(uint32_t param_types,
    TEE_Param params[4])
{
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INOUT,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE);
    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;
    BaseRequest* req = (BaseRequest*)params[0].memref.buffer;
    // DMSG("req->reqtype: %d", req->reqType, req->status);
    assert(params[0].memref.size == sizeof(EncIntBulkRequestData));
    int counter = 0;

    gcm_demo(param_types, params);

    // uint64_t duration =0,timer = 0;
    // uint64_t start,end;
    // uint32_t freq = read_cntfrq();
    while (true) {
        if (req->status != SENT)
            YIELD_PROCESSOR;
        else {
            LOAD_BARRIER;
#ifdef __TA_PROFILE
            start = read_cntpct();
#endif
            counter++;
            if (counter % 10000 == 0)
                DMSG("counter %d", counter++);

            handle_ops(req);

            if (req->resp != 0) {
                DMSG("TA error %d, %d\n", req->resp, counter);
            }

            STORE_BARRIER;
            req->status = DONE;

#ifdef __TA_PROFILE
            end = read_cntpct();
            duration = (end - start) * 1000000 / freq;
            timer += duration;
            DMSG("duration in us %d, end %d, start %d, freq %d,, avg %d", duration, end, start, freq, timer / counter);
#endif
        }

        if (req->status == 233) {
            DMSG("TA Exit %d\n", counter);
            break;
        }
    }

    return TEE_SUCCESS;
}

/* Decrypts byte array by aesgcm mode
 @input: uint8_t array - pointer to encrypted byte array
         size_t - length of encrypted  array
         uint8_t array - pointer to decrypted array
         size_t - length of decrypted array (length of array -
 IV_SIZE - TAG_SIZE)
 @return:
    * SGX_error, if there was an error during encryption/decryption
    0, otherwise
*/
int decrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len)
{

    // _print_hex("dec ", pSrc, src_len);

    size_t dst_len = 0;
    int resp = 0;
    // DMSG("before dec %x %d %x %d",pSrc,src_len, pDst, dst_len);

    // uint64_t duration =0,timer = 0;
    // uint64_t start,end;
    // uint32_t freq = read_cntfrq();
    // start = read_cntpct();

    // for (size_t i = 0; i < 1000; i++)
    // {
    // uint8_t *iv_pos = pSrc;
    // uint8_t *tag_pos = pSrc+IV_SIZE;
    // uint8_t *data_pos = pSrc+IV_SIZE+TAG_SIZE;
    // _print_hex("dec--enc-iv: ", (void *)iv_pos, IV_SIZE);
    // _print_hex("dec--enc-tag: ", (void *)tag_pos, TAG_SIZE);
    // _print_hex("dec--enc-txt: ", (void *)data_pos, exp_dst_len);
    if (src_len <= IV_SIZE + TAG_SIZE) {
        DMSG("error src len");
        return -1;
    }
    resp = gcm_decrypt(pSrc, src_len, pDst, &dst_len);
    // DMSG("after dec");
    assert(dst_len == exp_dst_len);

    // }

    // end= read_cntpct();
    // duration = (end - start) * 1000000 / freq;
    // timer += duration;
    // DMSG("1000 dec duration in us %d",duration);

    // memset(pDst, 0, dst_len);
    // memcpy(pDst, pSrc, dst_len);
    if (resp != 0) {
        _print_hex("dec from ", pSrc, src_len);
        _print_hex("dec to ", pDst, dst_len);
    }

    return resp;
}

/* Encrypts byte array by aesgcm mode
 @input: uint8_t array - pointer to a byte array
         size_t - length of the array
         uint8_t array - pointer to result array
         size_t - length of result array (IV_SIZE + length of array +
 TAG_SIZE)
 @return:
    * SGX_error, if there was an error during encryption/decryption
    0, otherwise
*/
int encrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len)
{

    size_t dst_len = exp_dst_len;
    int resp = 0;

    resp = gcm_encrypt(pSrc, src_len, pDst, &dst_len);
    // DMSG("after enc");
    assert(dst_len == exp_dst_len);

    return resp;
}

static TEE_Result loadkey(uint32_t param_types,
    TEE_Param params[4])
{
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INOUT,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE,
        TEE_PARAM_TYPE_NONE);
    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    return TEE_SUCCESS;
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused* sess_ctx,
    uint32_t cmd_id,
    uint32_t param_types, TEE_Param params[4])
{

    (void)&sess_ctx; /* Unused parameter */

    switch (cmd_id) {
    case TA_OPS_CMD_INC_VALUE:
        return inc_value(param_types, params);
    case TA_OPS_CMD_DEC_VALUE:
        return gcm_demo(param_types, params);
    case TA_OPS_CMD_OPS_PROCESS:
        return dec_value(param_types, params);
    case TA_OPS_CMD_LOAD_KEY:
        return loadkey(param_types, params);
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}