#pragma once

#if defined(TEE_CVM)
#include "ops_server.hpp"
#endif

#include <request_types.h>
#include <stdbool.h>

int enc_int32_calc(EncIntCalcRequestData* req);
int enc_int32_cmp(EncIntCmpRequestData* req);
int enc_int32_bulk(EncIntBulkRequestData* req);
