#pragma once

// #include "tools/timestamp.h"
#if defined(TEE_SGX)
#include <sgx/enclave.hpp>
#elif defined(TEE_TZ)
#include <trustzone/ops_ta.h>
#elif defined(TEE_SIM)
#include <simulate/sim.hpp>
#endif
#include <request_types.h>

int enc_timestamp_cmp(EncTimestampCmpRequestData* req);
int enc_timestamp_extract_year(EncTimestampExtractYearRequestData* req);
