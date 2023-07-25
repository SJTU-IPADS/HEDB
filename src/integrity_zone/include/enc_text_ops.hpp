#pragma once

#include <enc_types.h>

int enc_text_cmp(EncStr* left, EncStr* right, int* res);
int enc_text_concatenate(EncStr* left, EncStr* right, EncStr* res);
int enc_text_substring(EncStr* str, int32_t start, int32_t length, EncStr* res);
int enc_text_like(EncStr* str, EncStr* pattern, int* res);

int enc_text_encrypt(Str* in, EncStr* out);
int enc_text_decrypt(EncStr* in, Str* out);
