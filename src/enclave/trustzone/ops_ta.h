/*
 * Copyright (c) 2016-2017, Linaro Limited
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
#ifndef TA_OPS_H
#define TA_OPS_H

#include <defs.h>
#include <stdint.h>
#include <string.h>
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
// /*
//  * This UUID is generated with uuidgen command
//  * the ITU-T UUID generator at http://www.itu.int/ITU-T/asn1/uuid.html
//  */
// #define TA_OPS_UUID { 0x64974d40, 0x92cd, 0x4714,		{ 0x9e, 0x8f, 0x44, 0x2f, 0x3a, 0x4d, 0xb3, 0xaf} }

// /* The function IDs implemented in this TA */
// #define TA_OPS_CMD_INC_VALUE		0
// #define TA_OPS_CMD_DEC_VALUE		1
// #define TA_OPS_CMD_OPS_PROCESS      2
// #define TA_OPS_CMD_AES_GCM 			3
// #define TA_OPS_CMD_LOAD_KEY			4

/* --- below is MACROs for Requests */

#define CMD_INT64_PLUS 1
#define CMD_INT64_MINUS 2
#define CMD_INT64_MULT 3
#define CMD_INT64_DIV 4
#define CMD_INT64_CMP 5
#define CMD_INT64_ENC 6
#define CMD_INT64_DEC 7
#define CMD_INT64_EXP 8
#define CMD_INT64_MOD 9
#define CMD_INT32_SUM_BULK 10

#define CMD_FLOAT4_PLUS 101
#define CMD_FLOAT4_MINUS 102
#define CMD_FLOAT4_MULT 103
#define CMD_FLOAT4_DIV 104
#define CMD_FLOAT4_CMP 105
#define CMD_FLOAT4_ENC 106
#define CMD_FLOAT4_DEC 107
#define CMD_FLOAT4_EXP 108
#define CMD_FLOAT4_MOD 109
#define CMD_FLOAT4_SUM_BULK 110

#define CMD_STRING_CMP 201
#define CMD_STRING_ENC 202
#define CMD_STRING_DEC 203
#define CMD_STRING_SUBSTRING 204
#define CMD_STRING_CONCAT 205
#define CMD_STRING_LIKE 206

#define CMD_TIMESTAMP_CMP 150
#define CMD_TIMESTAMP_ENC 151
#define CMD_TIMESTAMP_DEC 152
#define CMD_TIMESTAMP_EXTRACT_YEAR 153

typedef unsigned char BYTE; // 1byte
typedef unsigned short WORD; // 2bytes
typedef unsigned long DWORD; // 4bytes

#define SHARED_BUF_SIZE 0x01000000

/* ERRORS */
#define ENCLAVE_IS_NOT_RUNNING -2
#define MEMORY_COPY_ERROR -3
#define ARITHMETIC_ERROR -4
#define MEMORY_ALLOCATION_ERROR -5
#define OUT_OF_THE_RANGE_ERROR -6
#define BASE64DECODER_ERROR -7
#define IS_NOT_INITIALIZE -8
#define NO_KEYS_STORAGE -9
#define NO_KEY_ID -10
#define NOT_IMPLEMENTED_OPERATOR -11
#define TOO_MANY_ELEMENTS_IN_BULK -12

int encrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len);
int decrypt_bytes(uint8_t* pSrc, size_t src_len, uint8_t* pDst, size_t exp_dst_len);

#endif /*TA_OPS_H*/
