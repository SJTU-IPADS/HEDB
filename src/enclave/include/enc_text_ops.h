#pragma once

// #include "tools/like_match.h"
// #include <string.h>

#if defined(TEE_SGX)
#include <sgx/enclave.hpp>
#elif defined(TEE_TZ)
#include <trustzone/ops_ta.h>
#elif defined(TEE_SIM)
#include <simulate/sim.hpp>
#endif
#include <kv.h>
#include <request_types.h>

int enc_text_cmp(EncStrCmpRequestData* req);
int enc_text_like(EncStrLikeRequestData* req);
int enc_text_concatenate(EncStrCalcRequestData* req);
int enc_text_substring(SubstringRequestData* req);
