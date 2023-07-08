#include "base64.h"
#include <enc_int_ops.hpp>
#include <enc_text_ops.hpp>
#include <extension.hpp>

extern bool clientMode;

#ifdef __cplusplus
extern "C" {
#endif
PG_FUNCTION_INFO_V1(enc_text_in);
PG_FUNCTION_INFO_V1(enc_text_out);
PG_FUNCTION_INFO_V1(enc_text_eq);
PG_FUNCTION_INFO_V1(enc_text_ne);
PG_FUNCTION_INFO_V1(enc_text_le);
PG_FUNCTION_INFO_V1(enc_text_lt);
PG_FUNCTION_INFO_V1(enc_text_ge);
PG_FUNCTION_INFO_V1(enc_text_gt);
PG_FUNCTION_INFO_V1(enc_text_cmp);
PG_FUNCTION_INFO_V1(enc_text_encrypt);
PG_FUNCTION_INFO_V1(enc_text_decrypt);
PG_FUNCTION_INFO_V1(enc_text_concatenate);
PG_FUNCTION_INFO_V1(enc_text_like);
PG_FUNCTION_INFO_V1(enc_text_notlike);
PG_FUNCTION_INFO_V1(enc_text_set_order);
PG_FUNCTION_INFO_V1(substring);
PG_FUNCTION_INFO_V1(varchar_to_enc_text);
#ifdef __cplusplus
}
#endif

/*
 * cstring_to_text_with_len
 *
 * Same as cstring_to_text except the caller specifies the string length;
 * the string need not be null_terminated.
 */
text* cstring_to_text_with_len(const char* s, int len)
{
    text* result = (text*)palloc0(len + VARHDRSZ);

    SET_VARSIZE(result, len + VARHDRSZ);
    memcpy(VARDATA(result), s, len);

    return result;
}

EncText* cstring_to_enctext_with_len(const char* s, uint32_t len)
{

    EncText* result = (EncText*)palloc0(ENCSTRLEN(len) + VARHDRSZ);
    Str str;

    str.len = len;
    memcpy(str.data, s, len);

    EncStr* estr = (EncStr*)VARDATA(result);
    estr->order = ORDER_NONE;

    enc_text_encrypt(&str, estr);

    SET_VARSIZE(result, ENCSTRLEN(len) + VARHDRSZ);

    // pfree(str); // TODO why i cannot free this pointer.
    return result;
}

// The input function converts a string to an enc_text element.
// @input: string
// @return: pointer to a structure describing enc_text element.
Datum enc_text_in(PG_FUNCTION_ARGS)
{
    char* s = PG_GETARG_CSTRING(0);
    EncText* result;
    result = (EncText*)cstring_to_enctext_with_len(s, strlen(s));
    PG_RETURN_POINTER(result);
}

// The output function converts an enc_text element to a string.
// @input: pointer to a structure describing enc_text element
// @return: string
Datum enc_text_out(PG_FUNCTION_ARGS)
{
    EncText* s = PG_GETARG_ENCTEXT_P(0);
    EncStr* estr = (EncStr*)VARDATA(s);

    if (clientMode == true) {
        Str str;
        enc_text_decrypt(estr, &str);
        char* res = (char*)palloc0(str.len + 1);
        memcpy(res, str.data, str.len);
        res[str.len] = '\0';
        // ereport(INFO, errmsg("out: order %d", estr->order));
        PG_RETURN_CSTRING(res);
    } else {
#define ENC_STRING_B64_LENGTH 1405 //((4 * n / 3) + 3) & ~3
        char base64_text[ENC_STRING_B64_LENGTH + 1] = { 0 };

        ToBase64Fast((const unsigned char*)&estr->enc_cstr, estr->len, base64_text, ENC_STRING_B64_LENGTH);
        // ereport(INFO, (errmsg("base64('%p') = %s", &estr->enc_cstr, base64_text)));
        PG_RETURN_CSTRING(base64_text);
    }
}

// @input: two strings
// @return: true, if strings are equal
//       false, otherwise
Datum enc_text_eq(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
#if 0
    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order == str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }
#endif

    if (enc_text_cmp(str1, str2, &ans) < 0) {
        exit(0);
    }

    if (ans == 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

// @input: two strings
// @return: true, if strings are not equal
//       false, otherwise
Datum enc_text_ne(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
#if 0
    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order != str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }
#endif
    if (enc_text_cmp(str1, str2, &ans) < 0) {
        exit(0);
    }

    if (ans != 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

// @input: two strings
// @return: true, if the first string is less or equal than the second one.
//       false, otherwise
Datum enc_text_le(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
    // int resp = 0;
#if 0
    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order <= str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }
#endif
    if (enc_text_cmp(str1, str2, &ans) < 0) {
        exit(0);
    }

    if (ans <= 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

// @input: two strings
// @return: true, if the first string is less than the second one.
//       false, otherwise
Datum enc_text_lt(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
#if 0
    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order < str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }
#endif
    if (enc_text_cmp(str1, str2, &ans) < 0) {
        exit(0);
    }

    if (ans < 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

// @input: two strings
// @return: true, if the first string is greater or equal than the second one.
//       false, otherwise
Datum enc_text_ge(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
#if 0
    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order >= str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }
#endif
    if (enc_text_cmp(str1, str2, &ans) < 0) {
        exit(0);
    }

    if (ans >= 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

// @input: two strings
// @return: true, if the first string is greater than the second one.
//       false, otherwise
Datum enc_text_gt(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
#if 0
    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order > str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }
#endif
    if (enc_text_cmp(str1, str2, &ans) < 0) {
        exit(0);
    }

    if (ans > 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

Datum enc_text_cmp(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    int ans = 0;
#if 0
    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order < str2->order) {
            ans = ORDER_NONE;
        }
        if (str1->order == str2->order) {
            ans = 0;
        }
        if (str1->order > str2->order) {
            ans = 1;
        }
        PG_RETURN_INT32(ans);
    }
#endif

    if (enc_text_cmp(str1, str2, &ans) < 0) {
        exit(0);
    }

    PG_RETURN_INT32(ans);
}

// The function encrypts the input string.
// IT'S A DEBUG FUNCTION SHOULD BE DELETED IN THE PRODUCT
// !!!!!!!!!!!!!!!!!!!!!!!!!
Datum enc_text_encrypt(PG_FUNCTION_ARGS)
{
    char* s = PG_GETARG_CSTRING(0);
    EncText* result;
    result = (EncText*)cstring_to_enctext_with_len(s, strlen(s));

    PG_RETURN_POINTER(result);
}

// The function decrypts the input enc_text element.
// IT'S A DEBUG FUNCTION SHOULD BE DELETED IN THE PRODUCT
// !!!!!!!!!!!!!!!!!!!!!!!!!
Datum enc_text_decrypt(PG_FUNCTION_ARGS)
{
    EncText* s = PG_GETARG_ENCTEXT_P(0);
    EncStr* estr = (EncStr*)VARDATA(s);
    Str str;
    enc_text_decrypt(estr, &str);

    char* res = (char*)palloc0(str.len + 1);
    memcpy(res, str.data, str.len);
    res[str.len] = '\0';
    PG_RETURN_CSTRING(res);
}

// @input: two strings
// @return: a result of a concatenation.
Datum enc_text_concatenate(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);

    int len = str1->len + str2->len - 2 * (IV_SIZE + TAG_SIZE);

    EncText* res = (EncText*)palloc0(ENCSTRLEN(len) + VARHDRSZ);
    EncStr* estr = (EncStr*)VARDATA(res);

    SET_VARSIZE(res, ENCSTRLEN(len) + VARHDRSZ);
    enc_text_concatenate(str1, str2, estr);

    PG_RETURN_POINTER(res);
}

Datum enc_text_like(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str = (EncStr*)VARDATA(s1);
    EncStr* pattern = (EncStr*)VARDATA(s2);
    int result = 0;
    if (enc_text_like(str, pattern, &result) < 0) {
        exit(0);
    }

    PG_RETURN_BOOL(result);
}

Datum enc_text_notlike(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str = (EncStr*)VARDATA(s1);
    EncStr* pattern = (EncStr*)VARDATA(s2);
    int result = 0;
    if (enc_text_like(str, pattern, &result) < 0) {
        exit(0);
    }

    PG_RETURN_BOOL(1 ^ result);
}

Datum enc_text_set_order(PG_FUNCTION_ARGS)
{
    EncText* s_in = PG_GETARG_ENCTEXT_P(0);
    int32_t order = PG_GETARG_INT32(1);
    EncStr* s_in_str = (EncStr*)VARDATA(s_in);
    EncText* result = (EncText*)palloc0(ENCSTRLEN(s_in_str->len) + VARHDRSZ);
    EncStr* s_out_str = (EncStr*)VARDATA(result);
    memcpy(s_out_str, s_in_str, ENCSTRLEN(s_in_str->len));
    s_out_str->order = order;
    SET_VARSIZE(result, ENCSTRLEN(s_in_str->len) + VARHDRSZ);

    PG_RETURN_POINTER(result);
}

// @input: string and two integers
// @return: the Substring specified by from and for.
Datum substring(PG_FUNCTION_ARGS)
{
    EncText* s = PG_GETARG_ENCTEXT_P(0);
    EncInt* start = PG_GETARG_ENCINT(1);
    EncInt* length = PG_GETARG_ENCINT(2);

    int sta, len;
    enc_int_decrypt(start, &sta);
    enc_int_decrypt(length, &len);
    EncStr* str = (EncStr*)VARDATA(s);

    EncText* res = (EncText*)palloc0(ENCSTRLEN(len) + VARHDRSZ);
    EncStr* estr = (EncStr*)VARDATA(res);
    SET_VARSIZE(res, ENCSTRLEN(len) + VARHDRSZ);
    enc_text_substring(str, start, length, estr);

    PG_RETURN_CSTRING(res);
}

// The input function converts a string to an enc_text element.
// @input: varying char
// @return: pointer to a structure describing enc_text element.
Datum varchar_to_enc_text(PG_FUNCTION_ARGS)
{
    Datum txt = PG_GETARG_DATUM(0);
    char* s = TextDatumGetCString(txt);
    EncText* result;
    result = (EncText*)cstring_to_enctext_with_len(s, strlen(s));
    PG_RETURN_POINTER(result);
}
