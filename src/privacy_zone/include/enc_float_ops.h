#pragma once

#if defined(TEE_CVM)
#include "ops_server.hpp"
#endif

#include <request_types.h>
#include <stdbool.h>

typedef union {
    float val;
    unsigned char bytes[FLOAT4_LENGTH];
} union_float4;

int enc_float32_calc(EncFloatCalcRequestData* req);
int enc_float32_cmp(EncFloatCmpRequestData* req);
int enc_float32_bulk(EncFloatBulkRequestData* req);
int enc_float32_eval_expr(EncFloatEvalExprRequestData* req);
