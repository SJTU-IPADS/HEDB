#pragma once

#include <enc_types.h>

int enc_timestamp_decrypt(EncTimestamp* src, TIMESTAMP* dst);
int enc_timestamp_encrypt(TIMESTAMP* src, EncTimestamp* dst);
int enc_timestamp_cmp(EncTimestamp* left, EncTimestamp* right, int* res);
int enc_timestamp_extract_year(EncTimestamp* in, EncInt* out);
