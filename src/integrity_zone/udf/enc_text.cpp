#include "base64.h"
#include <enc_int_ops.hpp>
#include <enc_text_ops.hpp>
#include <extension.hpp>

extern bool clientMode;

#ifdef __cplusplus
extern "C" {
#endif
PG_FUNCTION_INFO_V1(enc_text_encrypt);
PG_FUNCTION_INFO_V1(enc_text_decrypt);
PG_FUNCTION_INFO_V1(enc_text_in);
PG_FUNCTION_INFO_V1(enc_text_out);
PG_FUNCTION_INFO_V1(enc_text_eq);
PG_FUNCTION_INFO_V1(enc_text_ne);
PG_FUNCTION_INFO_V1(enc_text_le);
PG_FUNCTION_INFO_V1(enc_text_lt);
PG_FUNCTION_INFO_V1(enc_text_ge);
PG_FUNCTION_INFO_V1(enc_text_gt);
PG_FUNCTION_INFO_V1(enc_text_cmp);
PG_FUNCTION_INFO_V1(enc_text_concatenate);
PG_FUNCTION_INFO_V1(enc_text_like);
PG_FUNCTION_INFO_V1(enc_text_notlike);
PG_FUNCTION_INFO_V1(enc_text_set_order);
PG_FUNCTION_INFO_V1(substring);
#ifdef __cplusplus
}
#endif

static EncText* cstring_to_enctext_with_len(const char* s, uint32_t len)
{
    EncText* result = (EncText*)palloc0(ENCSTRLEN(len) + VARHDRSZ);

    Str str;
    str.len = len;
    memcpy(str.data, s, len);

    EncStr* estr = (EncStr*)VARDATA(result);
    int error = enc_text_encrypt(&str, estr);
    if (error) print_error("%s %d", __func__, error);

    estr->order = ORDER_NONE;

    SET_VARSIZE(result, ENCSTRLEN(len) + VARHDRSZ);
    // pfree(str); // TODO: why I cannot free this pointer?
    return result;
}

Datum enc_text_encrypt(PG_FUNCTION_ARGS)
{
    char* s = PG_GETARG_CSTRING(0);
    EncText* result;
    result = (EncText*)cstring_to_enctext_with_len(s, strlen(s));

    PG_RETURN_POINTER(result);
}

Datum enc_text_decrypt(PG_FUNCTION_ARGS)
{
    EncText* s = PG_GETARG_ENCTEXT_P(0);
    EncStr* estr = (EncStr*)VARDATA(s);
    Str str;

    int error = enc_text_decrypt(estr, &str);
    if (error) print_error("%s %d", __func__, error);

    char* res = (char*)palloc0(str.len + 1);
    memcpy(res, str.data, str.len);
    res[str.len] = '\0';
    PG_RETURN_CSTRING(res);
}

// The input function converts a string to an enc_text element.
// @input: string
// @return: pointer to a structure describing enc_text element.
Datum enc_text_in(PG_FUNCTION_ARGS)
{
    char* pIn = PG_GETARG_CSTRING(0);
    EncText* result;

    if (clientMode == true) {
        result = (EncText*)cstring_to_enctext_with_len(pIn, strlen(pIn));
    } else {
        EncStr* estr = (EncStr*)palloc0(ENC_STRING_B64_LENGTH);
        memset(estr, 0, ENC_STRING_B64_LENGTH);
        fromBase64(pIn, strlen(pIn), (unsigned char*)estr);

        size_t new_sz = estr->len + 12; // TODO: figure out where 12 is from
        result = (EncText*)palloc0(new_sz + VARHDRSZ);
        memcpy(VARDATA(result), estr, new_sz);
        SET_VARSIZE(result, new_sz + VARHDRSZ);

        pfree(estr);
    }
    PG_RETURN_POINTER(result);
}

// The output function converts an enc_text element to a string.
// @input: pointer to a structure describing enc_text element
// @return: string
Datum enc_text_out(PG_FUNCTION_ARGS)
{
    EncText* s = PG_GETARG_ENCTEXT_P(0);
    EncStr* estr = (EncStr*)VARDATA(s);
    // ereport(INFO, errmsg("enc_text_out: order %d", estr->order));

    if (clientMode == true) {
        Str str;
        int error = enc_text_decrypt(estr, &str);
        if (error) print_error("%s %d", __func__, error);
        char* res = (char*)palloc0(str.len + 1);
        memcpy(res, str.data, str.len);
        res[str.len] = '\0';
        PG_RETURN_CSTRING(res);
    } else {
        char base64_text[ENC_STRING_B64_LENGTH + 1] = { 0 };
        toBase64((const unsigned char*)&VARDATA(s), VARSIZE(s), base64_text);
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

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order == str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    int error = enc_text_cmp(str1, str2, &ans);
    if (error) print_error("%s %d", __func__, error);

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

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order != str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    int error = enc_text_cmp(str1, str2, &ans);
    if (error) print_error("%s %d", __func__, error);

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

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order <= str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    int error = enc_text_cmp(str1, str2, &ans);
    if (error) print_error("%s %d", __func__, error);

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

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order < str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    int error = enc_text_cmp(str1, str2, &ans);
    if (error) print_error("%s %d", __func__, error);

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

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order >= str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    int error = enc_text_cmp(str1, str2, &ans);
    if (error) print_error("%s %d", __func__, error);

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

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order > str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    int error = enc_text_cmp(str1, str2, &ans);
    if (error) print_error("%s %d", __func__, error);

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

    int error = enc_text_cmp(str1, str2, &ans);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_INT32(ans);
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
    int error = enc_text_concatenate(str1, str2, estr);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_POINTER(res);
}

Datum enc_text_like(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str = (EncStr*)VARDATA(s1);
    EncStr* pattern = (EncStr*)VARDATA(s2);
    int result = 0;
    int error = enc_text_like(str, pattern, &result);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_BOOL(result);
}

Datum enc_text_notlike(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str = (EncStr*)VARDATA(s1);
    EncStr* pattern = (EncStr*)VARDATA(s2);
    int result = 0;
    int error = enc_text_like(str, pattern, &result);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_BOOL(1 ^ result);
}

// @input: string and two integers
// @return: the Substring specified by from and for.
Datum substring(PG_FUNCTION_ARGS)
{
    EncText* s = PG_GETARG_ENCTEXT_P(0);
    EncStr* str = (EncStr*)VARDATA(s);

    EncInt* start = PG_GETARG_ENCINT(1);
    EncInt* length = PG_GETARG_ENCINT(2);

    int len = str->len - (IV_SIZE + TAG_SIZE);
    EncText* res = (EncText*)palloc0(ENCSTRLEN(len) + VARHDRSZ);
    EncStr* estr = (EncStr*)VARDATA(res);
    SET_VARSIZE(res, ENCSTRLEN(len) + VARHDRSZ);
    int error = enc_text_substring(str, start, length, estr);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_CSTRING(res);
}

Datum enc_text_set_order(PG_FUNCTION_ARGS)
{
    EncText* s_in = PG_GETARG_ENCTEXT_P(0);
    int32_t order = PG_GETARG_INT32(1);
    // ereport(INFO, errmsg("%s %d order %d", __func__, __LINE__, order));

    EncStr* s_in_str = (EncStr*)VARDATA(s_in);
    EncText* result = (EncText*)palloc0(ENCSTRLEN(s_in_str->len) + VARHDRSZ);
    EncStr* s_out_str = (EncStr*)VARDATA(result);
    memcpy(s_out_str, s_in_str, ENCSTRLEN(s_in_str->len));
    s_out_str->order = order;
    SET_VARSIZE(result, ENCSTRLEN(s_in_str->len) + VARHDRSZ);

    PG_RETURN_POINTER(result);
}
