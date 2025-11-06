#pragma once

#include <defs.h>
#include <enc_float_ops.h>
#include <enc_int_ops.h>
#include <enc_text_ops.h>
#include <enc_timestamp_ops.h>
#include <request_types.h>

#ifdef __cplusplus
extern "C" {
#endif

int handler(BaseRequest* req);

#ifdef __cplusplus
}
#endif
