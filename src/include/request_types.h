#pragma once
#include <enc_types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ReqStatus {
    NONE,
    SENT,
    DONE,
    EXIT
} ReqStat;

typedef struct
{
    volatile ReqStat status;
    int reqType;
    int resp;
} BaseRequest;

/* enc and dec req are for debug purpose. */
#define DEFINE_ENCTYPE_ENC_ReqData(enc_type, plain_type) \
    typedef struct {                                     \
        BaseRequest common;                              \
        plain_type plaintext;                            \
        enc_type ciphertext;                             \
    } enc_type##EncRequestData;

DEFINE_ENCTYPE_ENC_ReqData(EncInt, int);
DEFINE_ENCTYPE_ENC_ReqData(EncFloat, float);
DEFINE_ENCTYPE_ENC_ReqData(EncTimestamp, int64_t);
DEFINE_ENCTYPE_ENC_ReqData(EncStr, Str);

#define DEFINE_ENCTYPE_DEC_ReqData(enc_type, plain_type) \
    typedef struct {                                     \
        BaseRequest common;                              \
        enc_type ciphertext;                             \
        plain_type plaintext;                            \
    } enc_type##DecRequestData;

DEFINE_ENCTYPE_DEC_ReqData(EncInt, int);
DEFINE_ENCTYPE_DEC_ReqData(EncFloat, float);
DEFINE_ENCTYPE_DEC_ReqData(EncTimestamp, int64_t);
DEFINE_ENCTYPE_DEC_ReqData(EncStr, Str);

#define DEFINE_ENCTYPE_CMP_ReqData(enc_type) \
    typedef struct {                         \
        BaseRequest common;                  \
        enc_type left;                       \
        enc_type right;                      \
        int cmp;                             \
    } enc_type##CmpRequestData;

DEFINE_ENCTYPE_CMP_ReqData(EncInt);
DEFINE_ENCTYPE_CMP_ReqData(EncFloat);
DEFINE_ENCTYPE_CMP_ReqData(EncTimestamp);
DEFINE_ENCTYPE_CMP_ReqData(EncStr);
typedef EncStrCmpRequestData EncStrLikeRequestData;

#define DEFINE_ENCTYPE_CALC_ReqData(enc_type) \
    typedef struct {                          \
        BaseRequest common;                   \
        int op;                               \
        enc_type left;                        \
        enc_type right;                       \
        enc_type res;                         \
    } enc_type##CalcRequestData;

DEFINE_ENCTYPE_CALC_ReqData(EncInt);
DEFINE_ENCTYPE_CALC_ReqData(EncFloat);
DEFINE_ENCTYPE_CALC_ReqData(EncTimestamp);
DEFINE_ENCTYPE_CALC_ReqData(EncStr);

#define DEFINE_ENCTYPE_BULK_ReqData(enc_type) \
    typedef struct {                          \
        BaseRequest common;                   \
        int bulk_size;                        \
        enc_type items[BULK_SIZE];            \
        enc_type res;                         \
    } enc_type##BulkRequestData;

DEFINE_ENCTYPE_BULK_ReqData(EncInt);
DEFINE_ENCTYPE_BULK_ReqData(EncFloat);

#define DEFINE_ENCTYPE_EVALEXPR_ReqData(enc_type) \
    typedef struct {                              \
        BaseRequest common;                       \
        int arg_cnt;                              \
        Str expr;                                 \
        enc_type items[EXPR_MAX_SIZE];            \
        enc_type res;                             \
    } enc_type##EvalExprRequestData;

DEFINE_ENCTYPE_EVALEXPR_ReqData(EncInt);
DEFINE_ENCTYPE_EVALEXPR_ReqData(EncFloat);

#define DEFINE_ENCTYPE_1ARG_ReqData(arg1_type, res_type, typename) \
    typedef struct {                                               \
        BaseRequest common;                                        \
        arg1_type in;                                              \
        res_type res;                                              \
    } typename;

DEFINE_ENCTYPE_1ARG_ReqData(EncTimestamp, EncInt, EncTimestampExtractYearRequestData);

#define DEFINE_ENCTYPE_3ARG_ReqData(arg1_type, arg1_name, arg2_type, arg2_name, arg3_type, arg3_name, res_type, typename) \
    typedef struct {                                                                                                      \
        BaseRequest common;                                                                                               \
        arg1_type arg1_name;                                                                                              \
        arg2_type arg2_name;                                                                                              \
        arg3_type arg3_name;                                                                                              \
        res_type res;                                                                                                     \
    } typename;

DEFINE_ENCTYPE_3ARG_ReqData(EncStr, str, int32_t, start, int32_t, length, EncStr, SubstringRequestData);

#ifdef __cplusplus
}
#endif
