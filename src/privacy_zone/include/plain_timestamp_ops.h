#pragma once
#include "defs.h"

#ifdef __cplusplus
extern "C" {
#endif

int plain_timestamp_cmp(TIMESTAMP left, TIMESTAMP right);
int plain_timestamp_extract_year(TIMESTAMP t);

#ifdef __cplusplus
}
#endif
