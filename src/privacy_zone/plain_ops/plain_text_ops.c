#include <defs.h>
#include <like_match.h>
#include <plain_text_ops.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

int plain_text_cmp(char* lhs, uint32_t l_size, char* rhs, uint32_t r_size)
{
    return strcmp((const char*)lhs, (const char*)rhs);
}

int plain_text_like(char* lhs, uint32_t l_size, char* rhs, uint32_t r_size)
{
    return MatchText(lhs, l_size, rhs, r_size);
}

void plain_text_concat(char* left, uint32_t* l_size, char* right, uint32_t r_size)
{
    memcpy(left + *l_size, right, r_size);
    *l_size += r_size;
    left[*l_size] = '\0';
}

static void SubText(char* dst, char* src, int start, int length)
{
    for (int i = 0; i < length; i++) {
        dst[i] = src[start + i - 1];
    }
    dst[length] = '\0';
}

int plain_text_substring(char* str, int32_t start, int32_t length, char* substr)
{
    SubText(substr, str, start, length);
    return 0;
}
