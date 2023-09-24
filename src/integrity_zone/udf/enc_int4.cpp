#include "base64.h"
#include <enc_float_ops.hpp>
#include <enc_int_ops.hpp>
#include <extension.hpp>

extern bool clientMode;

#ifdef __cplusplus
extern "C" {
#endif
PG_FUNCTION_INFO_V1(enc_int4_encrypt);
PG_FUNCTION_INFO_V1(enc_int4_decrypt);
PG_FUNCTION_INFO_V1(enc_int4_in);
PG_FUNCTION_INFO_V1(enc_int4_out);
PG_FUNCTION_INFO_V1(enc_int4_add);
PG_FUNCTION_INFO_V1(enc_int4_sub);
PG_FUNCTION_INFO_V1(enc_int4_mult);
PG_FUNCTION_INFO_V1(enc_int4_div);
PG_FUNCTION_INFO_V1(enc_int4_pow);
PG_FUNCTION_INFO_V1(enc_int4_mod);
PG_FUNCTION_INFO_V1(enc_int4_cmp);
PG_FUNCTION_INFO_V1(enc_int4_eq);
PG_FUNCTION_INFO_V1(enc_int4_ne);
PG_FUNCTION_INFO_V1(enc_int4_lt);
PG_FUNCTION_INFO_V1(enc_int4_le);
PG_FUNCTION_INFO_V1(enc_int4_gt);
PG_FUNCTION_INFO_V1(enc_int4_ge);
PG_FUNCTION_INFO_V1(enc_int4_max);
PG_FUNCTION_INFO_V1(enc_int4_min);
PG_FUNCTION_INFO_V1(enc_int4_sum_bulk);
PG_FUNCTION_INFO_V1(enc_int4_avg_bulk);
PG_FUNCTION_INFO_V1(enc_int4_min_bulk);
PG_FUNCTION_INFO_V1(enc_int4_max_bulk);
#ifdef __cplusplus
}
#endif

Datum enc_int4_encrypt(PG_FUNCTION_ARGS)
{
    EncInt* out = (EncInt*)palloc0(sizeof(EncInt));
    int in = PG_GETARG_INT32(0);
    int error = enc_int_encrypt(in, out);
    if (error) print_error("%s %d", __func__, error);
    PG_RETURN_CSTRING(out);
}

Datum enc_int4_decrypt(PG_FUNCTION_ARGS)
{
    int ans = 0;
    EncInt* in = PG_GETARG_ENCINT(0);
    int error = enc_int_decrypt(in, &ans);
    if (error) print_error("%s %d", __func__, error);
    PG_RETURN_INT32(ans);
}

Datum enc_int4_in(PG_FUNCTION_ARGS)
{
    char* pIn = PG_GETARG_CSTRING(0);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    if (clientMode == true) { // from plain int4 to cipher int4
        int in = atoi(pIn);
        int error = enc_int_encrypt(in, result);
        if (error) print_error("%s %d", __func__, error);
    } else { // base64 decode to cipher int4
        fromBase64(pIn, strlen(pIn), (unsigned char*)result);
    }
    PG_RETURN_POINTER(result);
}

Datum enc_int4_out(PG_FUNCTION_ARGS)
{
    EncInt* in = PG_GETARG_ENCINT(0);

    if (clientMode == true) { // from cipher int4 to plain int4
        int out;
        char* str = (char*)palloc0(sizeof(EncInt)); // this length is not really meaningful

        int error = enc_int_decrypt(in, &out);
        if (error) print_error("%s %d", __func__, error);
        sprintf(str, "%d", out);
        // ereport(INFO, (errmsg("auto decryption('%p') = %d", in, out)));
        PG_RETURN_CSTRING(str);
    } else { // base64 encode of cipher int4
#define ENC_INT_B64_LENGTH 45 // ((4 * n / 3) + 3) & ~3
        char base64_int4[ENC_INT_B64_LENGTH + 1] = { 0 };

        toBase64((const unsigned char*)in, sizeof(EncInt), base64_int4);
        // ereport(INFO, (errmsg("base64('%p') = %s", in, base64_int4)));
        PG_RETURN_CSTRING(base64_int4);
    }
}

Datum enc_int4_add(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    int error = enc_int_add(left, right, result);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_sub(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    int error = enc_int_sub(left, right, result);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_mult(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    int error = enc_int_mult(left, right, result);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_div(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    int error = enc_int_div(left, right, result);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_pow(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    int error = enc_int_pow(left, right, result);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_mod(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    EncInt* result = (EncInt*)palloc0(sizeof(EncInt));

    int error = enc_int_mod(left, right, result);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_CSTRING(result);
}

Datum enc_int4_cmp(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;

    int error = enc_int_cmp(left, right, &res);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_INT32(res);
}

Datum enc_int4_eq(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    int error = enc_int_cmp(left, right, &res);
    if (error) print_error("%s %d", __func__, error);

    if (res == 0)
        cmp = true;
    else
        cmp = false;

    PG_RETURN_BOOL(cmp);
}

Datum enc_int4_ne(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    int error = enc_int_cmp(left, right, &res);
    if (error) print_error("%s %d", __func__, error);

    if (res == 0)
        cmp = false;
    else
        cmp = true;

    PG_RETURN_BOOL(cmp);
}

Datum enc_int4_lt(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    int error = enc_int_cmp(left, right, &res);
    if (error) print_error("%s %d", __func__, error);

    if (res == -1)
        cmp = true;
    else
        cmp = false;

    PG_RETURN_BOOL(cmp);
}

Datum enc_int4_le(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    int error = enc_int_cmp(left, right, &res);
    if (error) print_error("%s %d", __func__, error);

    if ((res == -1) || (res == 0))
        cmp = true;
    else
        cmp = false;

    PG_RETURN_BOOL(cmp);
}

Datum enc_int4_gt(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    int error = enc_int_cmp(left, right, &res);
    if (error) print_error("%s %d", __func__, error);

    if (res == 1)
        cmp = true;
    else
        cmp = false;

    PG_RETURN_BOOL(cmp);
}

Datum enc_int4_ge(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;
    bool cmp = false;

    int error = enc_int_cmp(left, right, &res);
    if (error) print_error("%s %d", __func__, error);

    if (res == 0 || res == 1)
        cmp = true;
    else
        cmp = false;
    PG_RETURN_BOOL(cmp);
}

/*
 * The function
 * @input: two enc_int4 values
 * @return: the larger enc_int4 value.
 */
Datum enc_int4_max(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;

    int error = enc_int_cmp(left, right, &res);
    if (error) print_error("%s %d", __func__, error);

    if (res == 1) {
        PG_RETURN_POINTER(left);
    } else {
        PG_RETURN_POINTER(right);
    }
}

/*
 * The function
 * @input: two enc_int4 values
 * @return: the smaller enc_int4 value.
 */
Datum enc_int4_min(PG_FUNCTION_ARGS)
{
    EncInt* left = PG_GETARG_ENCINT(0);
    EncInt* right = PG_GETARG_ENCINT(1);
    int res;

    int error = enc_int_cmp(left, right, &res);
    if (error) print_error("%s %d", __func__, error);

    if (res == 1) {
        PG_RETURN_POINTER(right);
    } else {
        PG_RETURN_POINTER(left);
    }
}

Datum enc_int4_sum_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    bool isnull;
    Datum value;

    EncInt* sum = (EncInt*)palloc0(sizeof(EncInt));
    EncInt sum_array[BULK_SIZE];
    int counter = 1;
    int error;

    // TODO: two copies happens here, for array of encint.
    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    ArrayIterator array_iterator = array_create_iterator(v, 0, my_extra);

    array_iterate(array_iterator, &value, &isnull);
    *sum = *DatumGetEncInt(value);
    sum_array[0] = *sum;
    while (array_iterate(array_iterator, &value, &isnull)) {
        sum_array[counter] = *DatumGetEncInt(value);
        counter++;
        if (counter == BULK_SIZE) {
            error = enc_int_sum_bulk(BULK_SIZE, sum_array, sum);
            if (error) print_error("%s %d", __func__, error);
            sum_array[0] = *sum;
            counter = 1;
        }
    }
    if (counter > 1) {
        error = enc_int_sum_bulk(counter, sum_array, sum);
        if (error) print_error("%s %d", __func__, error);
    }

    PG_RETURN_CSTRING(sum);
}

Datum enc_int4_avg_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    bool isnull;
    Datum value;

    EncInt sum;
    EncInt* res = (EncInt*)palloc0(sizeof(EncInt));
    EncInt sum_array[BULK_SIZE];
    EncInt num;
    EncInt unit; // cipher of '1'
    EncInt num_array[BULK_SIZE]; // nitems of '1'
    int counter; // sum will be at array[0]

    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    ArrayIterator array_iterator = array_create_iterator(v, 0, my_extra);

    array_iterate(array_iterator, &value, &isnull);
    sum = *DatumGetEncInt(value);
    sum_array[0] = sum;
    counter = 1;

    int error = enc_int_div(&sum_array[0], &sum_array[0], &unit); // get the cipher of '1'
    if (error) print_error("%s %d", __func__, error);

    for (int i = 0; i < BULK_SIZE; ++i) { // get nitems of '1'
        num_array[i] = unit;
    }
    while (array_iterate(array_iterator, &value, &isnull)) {
        sum_array[counter] = *DatumGetEncInt(value);
        num_array[counter] = unit;
        counter++;
        if (counter == BULK_SIZE) {
            error = enc_int_sum_bulk(BULK_SIZE, sum_array, &sum);
            if (error) print_error("%s %d", __func__, error);
            error = enc_int_sum_bulk(BULK_SIZE, num_array, &num);
            if (error) print_error("%s %d", __func__, error);
            sum_array[0] = sum;
            num_array[0] = num;
            counter = 1;
        }
    }

    if (counter > 1) {
        error = enc_int_sum_bulk(counter, sum_array, &sum);
        if (error) print_error("%s %d", __func__, error);
        error = enc_int_sum_bulk(counter, num_array, &num);
        if (error) print_error("%s %d", __func__, error);
    }
    error = enc_int_div(&sum, &num, res);
    if (error) print_error("%s %d", __func__, error);

    PG_RETURN_CSTRING(res);
}

Datum enc_int4_min_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    int ans = 0;
    ArrayIterator array_iterator;
    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    bool isnull;
    Datum value;

    EncInt* pMin = (EncInt*)palloc0(sizeof(EncInt));
    EncInt pTemp;
    array_iterator = array_create_iterator(v, 0, my_extra);
    array_iterate(array_iterator, &value, &isnull);

    memcpy(pMin, DatumGetCString(value), sizeof(EncInt));

    while (array_iterate(array_iterator, &value, &isnull)) {
        memcpy(&pTemp, DatumGetCString(value), sizeof(EncInt));

        int error = enc_int_cmp(pMin, &pTemp, &ans);
        if (error) print_error("%s %d", __func__, error);

        if (ans == 1)
            memcpy(pMin, &pTemp, sizeof(EncInt));
    }

    // pfree(pTemp);

    PG_RETURN_CSTRING(pMin);
}

Datum enc_int4_max_bulk(PG_FUNCTION_ARGS)
{
    ArrayType* v = PG_GETARG_ARRAYTYPE_P(0);
    int ans = 0;
    ArrayIterator array_iterator;
    ArrayMetaState* my_extra = (ArrayMetaState*)fcinfo->flinfo->fn_extra;
    bool isnull;
    Datum value;

    EncInt* pMax = (EncInt*)palloc0(sizeof(EncInt));
    EncInt pTemp;

    array_iterator = array_create_iterator(v, 0, my_extra);
    array_iterate(array_iterator, &value, &isnull);

    memcpy(pMax, DatumGetCString(value), sizeof(EncInt));

    while (array_iterate(array_iterator, &value, &isnull)) {
        memcpy(&pTemp, DatumGetCString(value), sizeof(EncInt));

        int error = enc_int_cmp(pMax, &pTemp, &ans);
        if (error) print_error("%s %d", __func__, error);

        if (ans == -1)
            memcpy(pMax, &pTemp, sizeof(EncInt));
    }

    // pfree(pTemp);

    PG_RETURN_CSTRING(pMax);
}