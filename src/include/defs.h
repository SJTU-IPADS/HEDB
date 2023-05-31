#pragma once

#include <stdint.h>

typedef unsigned char BYTE; // 1byte
typedef unsigned short WORD; // 2bytes
typedef unsigned long DWORD; // 4bytes

#define IV_SIZE 12
#define TAG_SIZE 16

#define SEALED_KEY_LENGTH 576

#define INT32_LENGTH sizeof(int)
#define ENC_INT32_GCD_LENGTH (INT32_LENGTH + IV_SIZE + TAG_SIZE)
#define ENC_INT32_OPE_LENGTH 0
#define ENC_INT32_LENGTH (ENC_INT32_OPE_LENGTH + ENC_INT32_GCD_LENGTH)

#define FLOAT4_LENGTH sizeof(float)
#define ENC_FLOAT4_GCD_LENGTH (FLOAT4_LENGTH + IV_SIZE + TAG_SIZE)
#define ENC_FLOAT4_OPE_LENGTH 0
#define ENC_FLOAT4_LENGTH (ENC_FLOAT4_GCD_LENGTH + ENC_FLOAT4_OPE_LENGTH)

#define TIMESTAMP int64_t
#define TIMESTAMP_LENGTH sizeof(int64_t)
#define ENC_TIMESTAMP_GCD_LENGTH (TIMESTAMP_LENGTH + IV_SIZE + TAG_SIZE)
#define ENC_TIMESTAMP_LENGTH (ENC_TIMESTAMP_GCD_LENGTH)

#define STRING_LENGTH 1024
#define ENC_STRING_GCD_LENGTH (STRING_LENGTH + IV_SIZE + TAG_SIZE)
#define ENC_STRING_OPE_LENGTH 0 // TODO: CHANGE to correct value
#define ENC_STRING_LENGTH (ENC_STRING_GCD_LENGTH + ENC_STRING_OPE_LENGTH)

#define BULK_SIZE (256) // 256
#define EXPR_MAX_SIZE (8) // 32
#define EXPR_STACK_MAX_SIZE (16) // 256

// errors
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

// COMMANDS
#define CMD_INT_PLUS 1
#define CMD_INT_MINUS 2
#define CMD_INT_MULT 3
#define CMD_INT_DIV 4
#define CMD_INT_CMP 5
#define CMD_INT_ENC 6
#define CMD_INT_DEC 7
#define CMD_INT_EXP 8
#define CMD_INT_MOD 9
#define CMD_INT_SUM_BULK 10

#define CMD_FLOAT_PLUS 101
#define CMD_FLOAT_MINUS 102
#define CMD_FLOAT_MULT 103
#define CMD_FLOAT_DIV 104
#define CMD_FLOAT_CMP 105
#define CMD_FLOAT_ENC 106
#define CMD_FLOAT_DEC 107
#define CMD_FLOAT_EXP 108
#define CMD_FLOAT_MOD 109
#define CMD_FLOAT_SUM_BULK 110
#define CMD_FLOAT_EVAL_EXPR 111

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

/* for Trustzone TA */
#ifdef TEE_TZ
#define TA_OPS_UUID                                        \
    {                                                      \
        0x64974d40, 0x92cd, 0x4714,                        \
        {                                                  \
            0x9e, 0x8f, 0x44, 0x2f, 0x3a, 0x4d, 0xb3, 0xaf \
        }                                                  \
    }

/* The function IDs implemented in this TA */
#define TA_OPS_CMD_INC_VALUE 0
#define TA_OPS_CMD_DEC_VALUE 1
#define TA_OPS_CMD_OPS_PROCESS 2
#define TA_OPS_CMD_AES_GCM 3
#define TA_OPS_CMD_LOAD_KEY 4
#endif