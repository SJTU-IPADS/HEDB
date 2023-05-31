/*
 * a simple UDF for text
 */
#include <enc_int_ops.hpp>
#include <enc_text_ops.hpp>
#include <extension.hpp>
#ifdef __cplusplus
extern "C" {
#endif
PG_FUNCTION_INFO_V1(pg_enc_text_in);
PG_FUNCTION_INFO_V1(pg_enc_text_out);
PG_FUNCTION_INFO_V1(pg_enc_text_eq);
PG_FUNCTION_INFO_V1(pg_enc_text_ne);
PG_FUNCTION_INFO_V1(pg_enc_text_le);
PG_FUNCTION_INFO_V1(pg_enc_text_lt);
PG_FUNCTION_INFO_V1(pg_enc_text_ge);
PG_FUNCTION_INFO_V1(pg_enc_text_gt);
PG_FUNCTION_INFO_V1(pg_enc_text_cmp);
PG_FUNCTION_INFO_V1(pg_enc_text_encrypt);
PG_FUNCTION_INFO_V1(pg_enc_text_decrypt);
PG_FUNCTION_INFO_V1(pg_enc_text_concatenate);
PG_FUNCTION_INFO_V1(pg_enc_text_like);
PG_FUNCTION_INFO_V1(pg_enc_text_notlike);
PG_FUNCTION_INFO_V1(pg_enc_text_set_order);
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
Datum pg_enc_text_in(PG_FUNCTION_ARGS)
{
    char* s = PG_GETARG_CSTRING(0);
    EncText* result;
    result = (EncText*)cstring_to_enctext_with_len(s, strlen(s));
    PG_RETURN_POINTER(result);
}

// The output function converts an enc_text element to a string.
// @input: pointer to a structure describing enc_text element
// @return: string
Datum pg_enc_text_out(PG_FUNCTION_ARGS)
{
    EncText* s = PG_GETARG_ENCTEXT_P(0);
    EncStr* estr = (EncStr*)VARDATA(s);
    Str str;
    // print_info("out before dec");
    enc_text_decrypt(estr, &str);
    // print_info("out after dec");
    char* res = (char*)palloc0(str.len + 1);
    memcpy(res, str.data, str.len);
    res[str.len] = '\0';
    // ereport(INFO, errmsg("out: order %d", estr->order));
    PG_RETURN_CSTRING(res);
}

// @input: two strings
// @return: true, if strings are equal
//       false, otherwise
Datum pg_enc_text_eq(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
    // int resp = 0;

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order == str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    if (enc_text_cmp(str1, str2, &ans) < 0) {
        // print_info("text eq");
        exit(0);
    }

    if (ans == 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

// @input: two strings
// @return: true, if strings are not equal
//       false, otherwise
Datum pg_enc_text_ne(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
    // int resp = 0;

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order != str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    if (enc_text_cmp(str1, str2, &ans) < 0) {
        // print_info("text ne");
        exit(0);
    }

    if (ans != 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

// @input: two strings
// @return: true, if the first string is less or equal than the second one.
//       false, otherwise
Datum pg_enc_text_le(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
    // int resp = 0;

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order <= str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    if (enc_text_cmp(str1, str2, &ans) < 0) {
        // print_info("text le");
        exit(0);
    }

    if (ans <= 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

// @input: two strings
// @return: true, if the first string is less than the second one.
//       false, otherwise
Datum pg_enc_text_lt(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
    // int resp = 0;

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order < str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    if (enc_text_cmp(str1, str2, &ans) < 0) {
        // print_info("text lt");
        exit(0);
    }

    if (ans < 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

// @input: two strings
// @return: true, if the first string is greater or equal than the second one.
//       false, otherwise
Datum pg_enc_text_ge(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
    // int resp = 0;

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order >= str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    if (enc_text_cmp(str1, str2, &ans) < 0) {
        // print_info("text ge");
        exit(0);
    }

    if (ans >= 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

// @input: two strings
// @return: true, if the first string is greater than the second one.
//       false, otherwise
Datum pg_enc_text_gt(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    bool cmp = false;
    int ans = 0;
    // int resp = 0;

    if (str1->order != ORDER_NONE && str2->order != ORDER_NONE) {
        if (str1->order > str2->order) {
            PG_RETURN_BOOL(true);
        } else {
            PG_RETURN_BOOL(false);
        }
    }

    if (enc_text_cmp(str1, str2, &ans) < 0) {
        exit(0);
    }

    if (ans > 0)
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

// @input: two strings
// @return: ORDER_NONE, if s1 < s2,
//        0, if s1 = s2,
//        1, if s1 > s2
Datum pg_enc_text_cmp(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);
    int ans = 0;
    // int resp = 0;

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

    if (enc_text_cmp(str1, str2, &ans) < 0) {
        // print_info("text cmp");
        exit(0);
    }

    PG_RETURN_INT32(ans);
}

// The function encrypts the input string.
// IT'S A DEBUG FUNCTION SHOULD BE DELETED IN THE PRODUCT
// !!!!!!!!!!!!!!!!!!!!!!!!!
Datum pg_enc_text_encrypt(PG_FUNCTION_ARGS)
{
    char* s = PG_GETARG_CSTRING(0);
    EncText* result;
    result = (EncText*)cstring_to_enctext_with_len(s, strlen(s));

    PG_RETURN_POINTER(result);
}

// The function decrypts the input enc_text element.
// IT'S A DEBUG FUNCTION SHOULD BE DELETED IN THE PRODUCT
// !!!!!!!!!!!!!!!!!!!!!!!!!
Datum pg_enc_text_decrypt(PG_FUNCTION_ARGS)
{
    // EncText* s1 = PG_GETARG_TEXT_PP(0);
    // // ereport(INFO, (errmsg("pg_enc_text_decrypt here!")));
    // char* src = VARDATA(s1);
    // size_t src_len = strlen(src);
    // char* pDst = (char*)palloc0((src_len + 1) * sizeof(char));
    // memcpy(pDst, src, src_len);
    // pDst[src_len] = '\0';
    // PG_RETURN_CSTRING(pDst);

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
Datum pg_enc_text_concatenate(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str1 = (EncStr*)VARDATA(s1);
    EncStr* str2 = (EncStr*)VARDATA(s2);

    // char ch[100];
    int len = str1->len + str2->len - 2 * (IV_SIZE + TAG_SIZE);

    // sprintf(ch, "before: str1->len %d, str2->len %d, len %d",str1->len, str2->len, len);
    // print_info(ch);
    EncText* res = (EncText*)palloc0(ENCSTRLEN(len) + VARHDRSZ);
    EncStr* estr = (EncStr*)VARDATA(res);

    SET_VARSIZE(res, ENCSTRLEN(len) + VARHDRSZ);
    enc_text_concatenate(str1, str2, estr);
    // sprintf(ch, "after: str1->len %d, str2->len %d, estr->len %d",str1->len, str2->len,estr->len);
    // print_info(ch);

    PG_RETURN_POINTER(res);
}

Datum pg_enc_text_like(PG_FUNCTION_ARGS)
{
    EncText* s1 = PG_GETARG_ENCTEXT_P(0);
    EncText* s2 = PG_GETARG_ENCTEXT_P(1);
    EncStr* str = (EncStr*)VARDATA(s1);
    EncStr* pattern = (EncStr*)VARDATA(s2);
    int result = 0;
    if (enc_text_like(str, pattern, &result) < 0) {
        // print_info("text like");
        exit(0);
    }

    PG_RETURN_BOOL(result);
}

Datum pg_enc_text_notlike(PG_FUNCTION_ARGS)
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

Datum pg_enc_text_set_order(PG_FUNCTION_ARGS)
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
// @return: the Substring specified by from and to.
Datum substring(PG_FUNCTION_ARGS)
{
    // NOT BE INVOKED
    EncText* s = PG_GETARG_ENCTEXT_P(0);
    EncInt* from = PG_GETARG_ENCINT(1);
    EncInt* to = PG_GETARG_ENCINT(2);

    int f, t;
    enc_int_decrypt(from, &f);
    enc_int_decrypt(to, &t);
    int len = t - f + 1;
    EncStr* str = (EncStr*)VARDATA(s);

    EncText* res = (EncText*)palloc0(ENCSTRLEN(len) + VARHDRSZ);
    EncStr* estr = (EncStr*)VARDATA(res);
    SET_VARSIZE(res, ENCSTRLEN(len) + VARHDRSZ);
    enc_text_substring(str, from, to, estr);

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
