#pragma once

#include <stdint.h>

typedef unsigned char BYTE; // 1byte
typedef unsigned short WORD; // 2bytes
typedef unsigned long DWORD; // 4bytes

#define IV_SIZE 12
#define TAG_SIZE 16

#define INT32_LENGTH sizeof(int)
#define ENC_INT32_GCD_LENGTH (INT32_LENGTH + IV_SIZE + TAG_SIZE)
#define ENC_INT32_LENGTH (ENC_INT32_GCD_LENGTH)

#define FLOAT4_LENGTH sizeof(float)
#define ENC_FLOAT4_GCD_LENGTH (FLOAT4_LENGTH + IV_SIZE + TAG_SIZE)
#define ENC_FLOAT4_LENGTH (ENC_FLOAT4_GCD_LENGTH)

#define TIMESTAMP int64_t
#define TIMESTAMP_LENGTH sizeof(int64_t)
#define ENC_TIMESTAMP_GCD_LENGTH (TIMESTAMP_LENGTH + IV_SIZE + TAG_SIZE)
#define ENC_TIMESTAMP_LENGTH (ENC_TIMESTAMP_GCD_LENGTH)

#define STRING_LENGTH 256 // TODO: change this value if needed
#define ENC_STRING_GCD_LENGTH (STRING_LENGTH + IV_SIZE + TAG_SIZE)
#define ENC_STRING_LENGTH (ENC_STRING_GCD_LENGTH)

#define BULK_SIZE (256) // 256
#define EXPR_MAX_SIZE (8) // 32
#define EXPR_STACK_MAX_SIZE (16) // 256

#define ENC_INT_B64_LENGTH 45 // ((4 * n / 3) + 3) & ~3
#define ENC_FLOAT_B64_LENGTH 45 // ((4 * n / 3) + 3) & ~3
#define ENC_STRING_B64_LENGTH 345 // ((4 * n / 3) + 3) & ~3
#define ENC_TIMESTAMP_B64_LENGTH 49 // ((4 * n / 3) + 3) & ~3

/* for ops opcode */
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

#define CMD_FLOAT_PLUS 21
#define CMD_FLOAT_MINUS 22
#define CMD_FLOAT_MULT 23
#define CMD_FLOAT_DIV 24
#define CMD_FLOAT_CMP 25
#define CMD_FLOAT_ENC 26
#define CMD_FLOAT_DEC 27
#define CMD_FLOAT_EXP 28
#define CMD_FLOAT_MOD 29
#define CMD_FLOAT_SUM_BULK 30
#define CMD_FLOAT_EVAL_EXPR 31

#define CMD_STRING_CMP 41
#define CMD_STRING_ENC 42
#define CMD_STRING_DEC 43
#define CMD_STRING_SUBSTRING 44
#define CMD_STRING_CONCAT 45
#define CMD_STRING_LIKE 46

#define CMD_TIMESTAMP_CMP 50
#define CMD_TIMESTAMP_ENC 51
#define CMD_TIMESTAMP_DEC 52
#define CMD_TIMESTAMP_EXTRACT_YEAR 53

/* for record-and-replay */
#define MAX_NAME_LENGTH 256
#define MAX_PARALLEL_WORKER_SIZE 16 // TODO: the database can only see one buffer allocated to it.
#define MAX_RECORDS_NUM (MAX_PARALLEL_WORKER_SIZE + 1)

#define ENC_FLOAT4_LENGTH_B64 BASE64_ENCODE_OUT_SIZE(ENC_FLOAT4_LENGTH)
#define ENC_INT32_LENGTH_B64 BASE64_ENCODE_OUT_SIZE(ENC_INT32_LENGTH)
#define ENC_STRING_LENGTH_B64 BASE64_ENCODE_OUT_SIZE(ENC_STRING_LENGTH)
#define ENC_TIMESTAMP_LENGTH_B64 BASE64_ENCODE_OUT_SIZE(ENC_TIMESTAMP_LENGTH)

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
