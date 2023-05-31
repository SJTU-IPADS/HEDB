#include <defs.h>
#include <like_match.h>
#include <plain_text_ops.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void SubText(char* dst, char* str, int from, int to)
{
    for (int i = 0; i < to - from + 1; i++) {
        dst[i] = str[from + i - 1];
    }
}

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

int plain_text_substring(char* str, int begin, int end, char* substr)
{

    SubText(substr, str, begin, end);
    return 0;
}
