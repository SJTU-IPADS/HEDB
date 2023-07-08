#pragma once

#if defined(TEE_CVM)
#include "ops_server.hpp"
#endif

#include <request_types.h>
#include <stdbool.h>

int enc_timestamp_cmp(EncTimestampCmpRequestData* req);
int enc_timestamp_extract_year(EncTimestampExtractYearRequestData* req);
