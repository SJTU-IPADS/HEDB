#pragma once
#include <stdint.h>

int plain_text_cmp(char* lhs, uint32_t l_size, char* rhs, uint32_t r_size);

int plain_text_like(char* lhs, uint32_t l_size, char* rhs, uint32_t r_size);

void plain_text_concat(char* left, uint32_t* l_size, char* right, uint32_t r_size);

int plain_text_substring(char* str, int begin, int end, char* substr);
