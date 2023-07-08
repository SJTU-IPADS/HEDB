#pragma once

#if defined(TEE_CVM)
#include "ops_server.hpp"
#endif

#include <request_types.h>
#include <stdbool.h>

int enc_text_cmp(EncStrCmpRequestData* req);
int enc_text_like(EncStrLikeRequestData* req);
int enc_text_concatenate(EncStrCalcRequestData* req);
int enc_text_substring(SubstringRequestData* req);
