#pragma once

#if defined(TEE_SGX)
#include <sgx/enclave.hpp>
#elif defined(TEE_TZ)
#include <trustzone/ops_ta.h>
#elif defined(TEE_SIM)
#include <simulate/sim.hpp>
#endif
#include <request_types.h>

int enc_int32_calc(EncIntCalcRequestData* req);
int enc_int32_cmp(EncIntCmpRequestData* req);
int enc_int32_bulk(EncIntBulkRequestData* req);
