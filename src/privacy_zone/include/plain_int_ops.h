#pragma once

#ifdef __cplusplus
extern "C" {
#endif

int plain_int32_calc(int reqType, int left, int right);
int plain_int32_cmp(int left, int right);
int plain_int32_bulk(int reqType, int size, int* array);

#ifdef __cplusplus
}
#endif
